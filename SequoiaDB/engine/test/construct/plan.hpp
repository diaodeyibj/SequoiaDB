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
*******************************************************************************/

#ifndef EXECUTIONPLAN_HPP_
#define EXECUTIONPLAN_HPP_

#include <string>
#include "core.hpp"

using namespace std;

const UINT32 SCALE_BASE = 100;

class executionPlan
{
   public:
      executionPlan()
      {
         _collection = "collection";
         _cs = "cs";
         _csNum = 1;
         _collectionNum = 1;
         _insert = 0;
         _update = 0;
         _delete = 0;
         _query = 0;
         _thread = 1;
         _scale = 0;
      }
      ~executionPlan(){}

   public:
      string _host ;
      UINT16 _port ;
      UINT64 _insert;
      UINT64 _update;
      UINT64 _delete;
      UINT64 _query;
      string _cs;
      UINT32 _csNum;
      string _collection;
      UINT32 _collectionNum;
      UINT32 _thread;
      UINT32 _scale;
};


#endif

