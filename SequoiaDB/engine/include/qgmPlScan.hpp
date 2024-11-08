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

   Source File Name = qgmPlScan.hpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains declare for QGM operators

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          04/09/2013  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef QGMPLSCAN_HPP_
#define QGMPLSCAN_HPP_

#include "qgmPlan.hpp"
#include "rtn.hpp"
#include "qgmSelector.hpp"

namespace engine
{
   struct _qgmConditionNode ;

   class _qgmPlScan : public _qgmPlan
   {
   public:
      //  element in orderby  does not include the collection name.
      _qgmPlScan( const qgmDbAttr &collection,

                  // element should be:
                  // field:alias
                  const qgmOPFieldVec &selector,
                  const BSONObj &orderby,
                  const BSONObj &hint,
                  INT64 numSkip,
                  INT64 numReturn,
                  const qgmField &alias,
                  _qgmConditionNode *condition ) ;

      virtual ~_qgmPlScan() ;

   public:
      virtual void close() ;

      virtual string toString() const ;

      const qgmDbAttr &collection(){ return _collection ; }

      virtual BOOLEAN canUseTrans() const ;

      virtual void    setClientVersion( INT32 version ) ;
      virtual INT32   getCatalogVersion() const ;

   protected:
      INT32 _executeOnData( _pmdEDUCB *eduCB ) ;

      INT32 _executeOnCoord( _pmdEDUCB *eduCB ) ;

   private:
      virtual INT32 _execute( _pmdEDUCB *eduCB ) ;

      virtual INT32 _fetchNext ( qgmFetchOut &next ) ;

      void _killContext() ;
      INT32 _fetch( const CHAR *&result ) ;

   protected:
      BSONObj _condition ;
      BOOLEAN _invalidPredicate ;
      SINT64 _contextID ;
      SDB_ROLE _dbRole ;

   private:
      qgmDbAttr _collection ;
      qgmSelector _selector ;
      BSONObj _orderby ;
      BSONObj _hint ;
      INT64 _skip ;
      INT64 _return ;

      /// if it is a data
      SDB_DMSCB *_dmsCB ;
      SDB_RTNCB *_rtnCB ;

      _qgmConditionNode *_conditionNode ;

      INT32   _clientVersion ;
      INT32   _catalogVersion ;
   } ;
   typedef class _qgmPlScan qgmPlScan ;
}

#endif

