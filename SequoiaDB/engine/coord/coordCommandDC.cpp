/*******************************************************************************

   Copyright (C) 2023-present SequoiaDB Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Source File Name = coordCommandDC.cpp

   Descriptive Name = Runtime Coord Common

   When/how to use: this program may be used on binary and text-formatted
   versions of runtime component. This file contains code logic for
   common functions for coordinator node.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          02/11/15    XJH Init
   Last Changed =

*******************************************************************************/

#include "coordCommandDC.hpp"
#include "msgMessage.hpp"
#include "coordUtil.hpp"
#include "catDef.hpp"
#include "pdTrace.hpp"
#include "coordTrace.hpp"
#include "coordCMDEventHandler.hpp"

using namespace bson ;

namespace engine
{

   /*
      _coordAlterDC implement
   */
   COORD_IMPLEMENT_CMD_AUTO_REGISTER( _coordAlterDC,
                                      CMD_NAME_ALTER_DC,
                                      FALSE ) ;
   _coordAlterDC::_coordAlterDC()
   {
   }

   _coordAlterDC::~_coordAlterDC()
   {
   }

   INT32 _coordAlterDC::execute( MsgHeader *pMsg,
                                 pmdEDUCB *cb,
                                 INT64 &contextID,
                                 rtnContextBuf *buf )
   {
      INT32 rc = SDB_OK ;
      CoordGroupList datagroups ;
      CoordGroupList allgroups ;
      vector< BSONObj > replyObjs ;
      const CHAR *pAction = NULL ;

      // fill default-reply
      contextID                        = -1 ;

      MsgOpQuery *pAttachMsg           = (MsgOpQuery *)pMsg ;
      pAttachMsg->header.opCode        = MSG_CAT_ALTER_IMAGE_REQ ;

      // extrace query msg
      {
         const CHAR *pQuery = NULL ;
         rc = msgExtractQuery( (const CHAR*)pMsg, NULL, NULL, NULL, NULL,
                               &pQuery, NULL, NULL, NULL ) ;
         PD_RC_CHECK( rc, PDERROR, "Extract command[%s] msg failed, rc: %d",
                      getName(), rc ) ;
         try
         {
            BSONObj objQuery( pQuery ) ;
            BSONElement eleAction = objQuery.getField( FIELD_NAME_ACTION ) ;
            if ( String != eleAction.type() )
            {
               PD_LOG( PDERROR, "The field[%s] is not valid in command[%s]'s "
                       "param[%s]", FIELD_NAME_ACTION, getName(),
                       objQuery.toString().c_str() ) ;
               rc = SDB_INVALIDARG ;
               goto error ;
            }
            pAction = eleAction.valuestr() ;
         }
         catch( std::exception &e )
         {
            rc = SDB_INVALIDARG ;
            PD_LOG( PDERROR, "Parse command[%s]'s param occur exception: %s",
                    getName(), e.what() ) ;
            goto error ;
         }
      }

      // 1. execute on catalog
      rc = executeOnCataGroup( pMsg, cb, &datagroups, &replyObjs, TRUE, NULL, buf ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to execute %s:%s on catalog node, rc: %d",
                 getName(), pAction, rc ) ;
         goto error ;
      }

      // 2. execute on the special groups or special nodes, ignore error
      pAttachMsg->header.opCode        = MSG_BS_QUERY_REQ ;

      // These commands will return all groups, no need to update group again
      if ( 0 == ossStrcasecmp( CMD_VALUE_NAME_SET_ACTIVE_LOCATION, pAction ) ||
           0 == ossStrcasecmp( CMD_VALUE_NAME_SET_LOCATION, pAction ) ||
           0 == ossStrcasecmp( CMD_VALUE_NAME_START_MAINTENANCE_MODE, pAction ) ||
           0 == ossStrcasecmp( CMD_VALUE_NAME_STOP_MAINTENANCE_MODE, pAction ) )
      {
         coordNodeCMDHelper helper ;
         BOOLEAN hasFailedGroup = FALSE ;

         // Remove failed groups in allgropus
         rc = coordRemoveFailedGroup( datagroups, hasFailedGroup, replyObjs ) ;
         if ( rc )
         {
            PD_LOG( PDWARNING, "Failed to parse failed group list, rc: %d", rc ) ;
         }
         rc = hasFailedGroup ? SDB_COORD_NOT_ALL_DONE : SDB_OK ;

         helper.notify2NodesByGroups( _pResource, datagroups, cb ) ;
      }
      else
      {
         // update all groups
         rc = _pResource->updateGroupList( allgroups, cb, NULL, FALSE, TRUE, TRUE ) ;
         if ( rc )
         {
            PD_LOG( PDWARNING, "Failed to update all group list, rc: %d", rc ) ;
            rc = SDB_OK ;
         }

         if ( 0 == ossStrcasecmp( CMD_VALUE_NAME_ENABLE_READONLY, pAction ) ||
              0 == ossStrcasecmp( CMD_VALUE_NAME_DISABLE_READONLY, pAction ) ||
              0 == ossStrcasecmp( CMD_VALUE_NAME_ACTIVATE, pAction ) ||
              0 == ossStrcasecmp( CMD_VALUE_NAME_DEACTIVATE, pAction ) )
         {
            _executeByNodes( pMsg, cb, allgroups, pAction, buf ) ;
         }
         else
         {
            _executeByGroups( pMsg, cb, allgroups, pAction, buf ) ;
         }
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _coordAlterDC::_executeByGroups( MsgHeader *pMsg,
                                          pmdEDUCB *cb,
                                          CoordGroupList &groupLst,
                                          const CHAR *pAction,
                                          rtnContextBuf *buf )
   {
      INT32 rc = SDB_OK ;

      rc = executeOnDataGroup( pMsg, cb, groupLst,
                               TRUE, NULL, NULL, NULL, buf ) ;
      if ( rc )
      {
         PD_LOG( PDWARNING, "Failed to execute %s:%s on data groups, "
                 "rc: %d", getName(), pAction, rc ) ;
      }

      return rc ;
   }

   INT32 _coordAlterDC::_executeByNodes( MsgHeader *pMsg,
                                         pmdEDUCB *cb,
                                         CoordGroupList &groupLst,
                                         const CHAR *pAction,
                                         rtnContextBuf *buf )
   {
      INT32 rc = SDB_OK ;
      SET_ROUTEID nodes ;
      ROUTE_RC_MAP faileds ;
      SET_ROUTEID sucNodes ;

      rc = coordGetGroupNodes( _pResource, cb, BSONObj(), NODE_SEL_ALL,
                               groupLst, nodes, NULL, FALSE ) ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Get group nodes failed, rc: %d", rc ) ;
         goto error ;
      }

      rc = executeOnNodes( pMsg, cb, nodes, faileds, &sucNodes, NULL, NULL ) ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Failed to execute %s:%s on data nodes, "
                 "rc: %d", getName(), pAction, rc ) ;
         goto error ;
      }

   done:
      if ( ( rc || faileds.size() > 0 ) && buf )
      {
         *buf = _rtnContextBuf( coordBuildErrorObj( _pResource, rc,
                                                    cb, &faileds,
                                                    sucNodes.size() ) ) ;
      }
      return rc ;
   error:
      goto done ;
   }

   /*
      _coordGetDCInfo implement
   */
   COORD_IMPLEMENT_CMD_AUTO_REGISTER( _coordGetDCInfo,
                                      CMD_NAME_GET_DCINFO,
                                      TRUE ) ;
   _coordGetDCInfo::_coordGetDCInfo()
   {
   }

   _coordGetDCInfo::~_coordGetDCInfo()
   {
   }

   INT32 _coordGetDCInfo::_preProcess( rtnQueryOptions &queryOpt,
                                       string &clName,
                                       BSONObj &outSelector )
   {
      clName = CAT_SYSDCBASE_COLLECTION_NAME ;
      queryOpt.setQuery( BSON( FIELD_NAME_TYPE << CAT_BASE_TYPE_GLOBAL_STR ) ) ;
      return SDB_OK ;
   }

}

