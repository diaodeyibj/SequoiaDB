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

   Source File Name = utilSet.hpp

   Descriptive Name =

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          14/04/2016  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef UTIL_SET_HPP_
#define UTIL_SET_HPP_

#include "core.hpp"
#include "oss.hpp"
#include "ossMem.hpp"
#include "ossUtil.hpp"
#include "ossMemPool.hpp"

using namespace std ;

#define UTIL_SET_DEFAULT_STACK_SIZE          4

namespace engine
{
   template <typename T, UINT32 stackSize = UTIL_SET_DEFAULT_STACK_SIZE >
   class _utilSet : public SDBObject
   {
   public:
      _utilSet()
      :_pSet( NULL ),
       _eleSize( 0 )
      {
      }

      ~_utilSet()
      {
         clear( TRUE ) ;
      }

   public:
      class iterator
      {
         friend class _utilSet< T, stackSize > ;
         public:
            iterator()
            {
               _pData      = NULL ;
               _pSrc       = NULL ;
               _pEleSize   = NULL ;
            }
            iterator( const iterator &rhs )
            {
               _pData      = rhs._pData ;
               _pSrc       = rhs._pSrc ;
               _pEleSize   = rhs._pEleSize ;
               _it         = rhs._it ;
            }
            BOOLEAN operator== ( const iterator &rhs ) const
            {
               if ( _pData && rhs._pData )
               {
                  /// left, right is end
                  BOOLEAN leftEnd = _pData >= _pSrc + *_pEleSize ?
                                    TRUE : FALSE ;
                  BOOLEAN rightEnd = rhs._pData > rhs._pSrc + *(rhs._pEleSize) ?
                                     TRUE : FALSE ;
                  /// both end,equal
                  if ( leftEnd && rightEnd &&
                       _pSrc == rhs._pSrc &&
                       _pEleSize == rhs._pEleSize )
                  {
                     return TRUE ;
                  }
                  return _pData == rhs._pData ? TRUE : FALSE ;
               }
               else if ( !_pData && !rhs._pData )
               {
                  return _it == rhs._it ? TRUE : FALSE ;
               }
               return FALSE ;
            }
            BOOLEAN operator!= ( const iterator &rhs ) const
            {
               return this->operator==( rhs ) ? FALSE : TRUE ;
            }
            iterator& operator= ( const iterator &rhs )
            {
               _pData         = rhs._pData ;
               _pSrc          = rhs._pSrc ;
               _pEleSize      = rhs._pEleSize ;
               _it            = rhs._it ;
               return *this ;
            }
            const T& operator* () const
            {
               if ( _pData )
               {
                  return *_pData ;
               }
               return *_it ;
            }
            iterator& operator++ ()
            {
               if ( _pData )
               {
                  ++_pData ;
               }
               else
               {
                  ++_it ;
               }
               return *this ;
            }
            iterator& operator++ ( int )
            {
               if ( _pData )
               {
                  _pData++ ;
               }
               else
               {
                  _it++ ;
               }
               return *this ;
            }
            iterator& operator-- ()
            {
               if ( _pData )
               {
                  --_pData ;
               }
               else
               {
                  --_it ;
               }
               return *this ;
            }
            iterator& operator-- ( int )
            {
               if ( _pData )
               {
                  _pData-- ;
               }
               else
               {
                  _it-- ;
               }
               return *this ;
            }
            iterator& operator+ ( UINT32 step )
            {
               if ( _pData )
               {
                  _pData += step ;
               }
               else
               {
                  _it += step ;
               }
               return *this ;
            }
            iterator& operator- ( UINT32 step )
            {
               if ( _pData )
               {
                  _pData -= step ;
               }
               else
               {
                  _it -= step ;
               }
               return *this ;
            }

         protected:
            iterator( T* pData, T *pSrc, UINT32 *pEleSize )
            {
               _pData         = pData ;
               _pSrc          = pSrc ;
               _pEleSize      = pEleSize ;
            }
            iterator( typename ossPoolSet<T>::iterator it )
            {
               _pData         = NULL ;
               _pSrc          = NULL ;
               _pEleSize      = NULL ;
               _it            = it ;
            }

         private:
            T*                                  _pData ;
            T*                                  _pSrc ;
            UINT32*                             _pEleSize ;
            typename ossPoolSet<T>::iterator    _it ;
      } ;

      class const_iterator
      {
         friend class _utilSet< T, stackSize > ;
         public:
            const_iterator()
            {
               _pData      = NULL ;
               _pSrc       = NULL ;
               _pEleSize   = NULL ;
            }
            const_iterator( const const_iterator &rhs )
            {
               _pData      = rhs._pData ;
               _pSrc       = rhs._pSrc ;
               _pEleSize   = rhs._pEleSize ;
               _it         = rhs._it ;
            }
            const_iterator( const iterator &rhs )
            {
               _pData      = rhs._pData ;
               _pSrc       = rhs._pSrc ;
               _pEleSize   = rhs._pEleSize ;
               _it         = rhs._it ;
            }
            BOOLEAN operator== ( const const_iterator &rhs ) const
            {
               if ( _pData && rhs._pData )
               {
                  /// left, right is end
                  BOOLEAN leftEnd = _pData >= _pSrc + *_pEleSize ?
                                    TRUE : FALSE ;
                  BOOLEAN rightEnd = rhs._pData > rhs._pSrc + *(rhs._pEleSize) ?
                                     TRUE : FALSE ;
                  /// both end,equal
                  if ( leftEnd && rightEnd &&
                       _pSrc == rhs._pSrc &&
                       _pEleSize == rhs._pEleSize )
                  {
                     return TRUE ;
                  }
                  return _pData == rhs._pData ? TRUE : FALSE ;
               }
               else if ( !_pData && !rhs._pData )
               {
                  return _it == rhs._it ? TRUE : FALSE ;
               }
               return FALSE ;
            }
            BOOLEAN operator!= ( const const_iterator &rhs ) const
            {
               return this->operator==( rhs ) ? FALSE : TRUE ;
            }
            const_iterator& operator= ( const const_iterator &rhs )
            {
               _pData         = rhs._pData ;
               _pSrc          = rhs._pSrc ;
               _pEleSize      = rhs._pEleSize ;
               _it            = rhs._it ;
               return *this ;
            }
            const T& operator* () const
            {
               if ( _pData )
               {
                  return *_pData ;
               }
               return *_it ;
            }
            const_iterator& operator++ ()
            {
               if ( _pData )
               {
                  ++_pData ;
               }
               else
               {
                  ++_it ;
               }
               return *this ;
            }
            const_iterator& operator++ ( int )
            {
               if ( _pData )
               {
                  _pData++ ;
               }
               else
               {
                  _it++ ;
               }
               return *this ;
            }
            const_iterator& operator-- ()
            {
               if ( _pData )
               {
                  --_pData ;
               }
               else
               {
                  --_it ;
               }
               return *this ;
            }
            const_iterator& operator-- ( int )
            {
               if ( _pData )
               {
                  _pData-- ;
               }
               else
               {
                  _it-- ;
               }
               return *this ;
            }
            const_iterator& operator+ ( UINT32 step )
            {
               if ( _pData )
               {
                  _pData += step ;
               }
               else
               {
                  _it += step ;
               }
               return *this ;
            }
            const_iterator& operator- ( UINT32 step )
            {
               if ( _pData )
               {
                  _pData -= step ;
               }
               else
               {
                  _it -= step ;
               }
               return *this ;
            }

         protected:
            const_iterator( const T* pData, const T *pSrc,
                            const UINT32 *pEleSize )
            {
               _pData         = pData ;
               _pSrc          = pSrc ;
               _pEleSize      = pEleSize ;
            }
            const_iterator( typename ossPoolSet<T>::const_iterator it )
            {
               _pData         = NULL ;
               _pSrc          = NULL ;
               _pEleSize      = NULL ;
               _it            = it ;
            }

         private:
            const T*                               _pData ;
            const T*                               _pSrc ;
            const UINT32*                          _pEleSize ;
            typename ossPoolSet<T>::const_iterator _it ;
      } ;

   public:
      OSS_INLINE UINT32 size() const
      {
         if ( _pSet )
         {
            return _pSet->size() ;
         }
         return _eleSize ;
      }

      OSS_INLINE BOOLEAN empty() const
      {
         if ( _pSet )
         {
            return _pSet->empty() ? TRUE : FALSE ;
         }
         return 0 == _eleSize ? TRUE : FALSE ;
      }

      OSS_INLINE iterator begin()
      {
         if ( _pSet )
         {
            return iterator( _pSet->begin() ) ;
         }
         return iterator( _staticBuf, _staticBuf, &_eleSize ) ;
      }

      OSS_INLINE const_iterator begin() const
      {
         if ( _pSet )
         {
            return const_iterator( _pSet->begin() ) ;
         }
         return const_iterator( _staticBuf, _staticBuf, &_eleSize ) ;
      }

      OSS_INLINE iterator end()
      {
         if ( _pSet )
         {
            return iterator( _pSet->end() ) ;
         }
         return iterator( &_staticBuf[ stackSize ], _staticBuf, &_eleSize ) ;
      }

      OSS_INLINE const_iterator end() const
      {
         if ( _pSet )
         {
            return const_iterator( _pSet->end() ) ;
         }
         return const_iterator( &_staticBuf[ stackSize ], _staticBuf,
                                &_eleSize ) ;
      }

      OSS_INLINE void erase( iterator position )
      {
         if ( _pSet )
         {
            _pSet->erase( position._it ) ;
            shift2Stack() ;
         }
         else if ( _eleSize > 0 )
         {
            --_eleSize ;
            UINT32 pos = position._pData - _staticBuf ;
            for ( UINT32 i = pos ; i < _eleSize ; ++i )
            {
               _staticBuf[ i ] = _staticBuf[ i + 1 ] ;
            }
         }
      }

      OSS_INLINE UINT32 erase( const T& value )
      {
         if ( _pSet )
         {
            UINT32 count = (UINT32)_pSet->erase( value ) ;
            if ( count > 0 )
            {
               shift2Stack() ;
            }
            return count ;
         }
         else if ( _eleSize > 0 )
         {
            UINT32 pos = _findInStackBuf( value ) ;
            if ( pos != this->npos )
            {
               --_eleSize ;
               for ( UINT32 i = pos ; i < _eleSize ; ++i )
               {
                  _staticBuf[ i ] = _staticBuf[ i + 1 ] ;
               }
               return 1 ;
            }
         }
         return 0 ;
      }

      OSS_INLINE void erase( iterator first, iterator last )
      {
         if ( _pSet )
         {
            _pSet->erase( first._it, last._it ) ;
            shift2Stack() ;
         }
         else if ( _eleSize > 0 )
         {
            UINT32 b = first._pData - _staticBuf ;
            UINT32 e = last._pData - _staticBuf ;

            UINT32 i = b ;
            UINT32 j = b ;
            UINT32 tmpSize = _eleSize ;
            for ( ; i < tmpSize ; ++i )
            {
               if ( i < e )
               {
                  --_eleSize ;
                  continue ;
               }
               _staticBuf[ j++ ] = _staticBuf[ i ] ;
            }
         }
      }

      OSS_INLINE void clear( BOOLEAN resetMem = TRUE )
      {
         if ( _pSet )
         {
            if ( resetMem )
            {
               delete _pSet ;
               _pSet = NULL ;
            }
            else
            {
               _pSet->clear() ;
            }
         }
         _eleSize = 0 ;
      }

      OSS_INLINE pair<iterator,BOOLEAN> insert( const T& val )
      {
         if ( SDB_OK != _ensureSpace( _eleSize + 1 ) )
         {
            return pair<iterator, BOOLEAN>( end(), FALSE ) ;
         }

         if ( _pSet )
         {
            pair< typename ossPoolSet<T>::iterator, bool > tmp = _pSet->insert( val ) ;
            return pair<iterator, BOOLEAN>( iterator( tmp.first ),
                                            tmp.second ? TRUE : FALSE ) ;
         }
         else if ( 0 == _eleSize )
         {
            _staticBuf[ 0 ] = val ;
            ++_eleSize ;
            return pair<iterator, BOOLEAN>( begin(), TRUE ) ;
         }
         else
         {
            UINT32 pos = _eleSize ;
            while( pos > 0 )
            {
               if ( val < _staticBuf[ pos - 1 ] )
               {
                  _staticBuf[ pos ] = _staticBuf[ pos - 1 ] ;
                  --pos ;
               }
               else if ( _staticBuf[ pos - 1 ] < val )
               {
                  /// find the position
                  _staticBuf[ pos ] = val ;
                  ++_eleSize ;
                  return pair<iterator, BOOLEAN>( iterator( &_staticBuf[ pos ],
                                                            _staticBuf,
                                                            &_eleSize ),
                                                  TRUE ) ;
               }
               else
               {
                  /// restore
                  while( pos < _eleSize )
                  {
                     _staticBuf[ pos ] = _staticBuf[ pos + 1 ] ;
                     ++pos ;
                  }
                  _staticBuf[ pos ] = T() ;

                  return pair<iterator, BOOLEAN>( end(), FALSE ) ;
               }
            }
            /// insert to the begin
            _staticBuf[ 0 ] = val ;
            ++_eleSize ;
            return pair<iterator, BOOLEAN>( begin(), TRUE ) ;
         }
      }

      OSS_INLINE _utilSet<T, stackSize>& operator= ( const _utilSet<T, stackSize> &rhs )
      {
         UINT32 rSize = rhs.size() ;

         /// clear self
         clear( TRUE ) ;
         /// alloc space
         _ensureSpace( rSize ) ;
         /// copy all elements
         const_iterator it = rhs.begin() ;
         while( it != rhs.end() )
         {
            insert( *it ) ;
            ++it ;
         }
         return *this ;
      }

      OSS_INLINE BOOLEAN operator== ( const _utilSet<T, stackSize> &rhs )
      {
         if ( rhs.size() != size() )
         {
            return FALSE ;
         }

         iterator itThis = begin() ;
         const_iterator itRhs = rhs.begin() ;
         while( itRhs != rhs.end() )
         {
            if ( *itRhs < *itThis || *itThis < *itRhs )
            {
               return FALSE ;
            }
            ++itThis ;
            ++itRhs ;
         }
         return TRUE ;
      }

      OSS_INLINE BOOLEAN operator!= ( const _utilSet<T, stackSize> &rhs )
      {
         return this->operator==( rhs ) ? FALSE : TRUE ;
      }

      OSS_INLINE iterator find( const T& val )
      {
         if ( _pSet )
         {
            return iterator( _pSet->find( val ) ) ;
         }
         UINT32 pos = _findInStackBuf( val ) ;
         if ( this->npos == pos )
         {
            return end() ;
         }
         return iterator( &_staticBuf[ pos ], _staticBuf, &_eleSize ) ;
      }

      OSS_INLINE const_iterator find( const T& val ) const
      {
         if ( _pSet )
         {
            return const_iterator( _pSet->find( val ) ) ;
         }
         UINT32 pos = _findInStackBuf( val ) ;
         if ( this->npos == pos )
         {
            return end() ;
         }
         return const_iterator( &_staticBuf[ pos ], _staticBuf, &_eleSize ) ;
      }

      OSS_INLINE UINT32 count( const T& val ) const
      {
         if ( _pSet )
         {
            return (UINT32)_pSet->count( val ) ;
         }
         return _findInStackBuf( val ) == this->npos ? 0 : 1 ;
      }

      OSS_INLINE iterator lower_bound( const T& val ) const
      {
         if ( _pSet )
         {
            return iterator( _pSet->low_bound( val ) ) ;
         }
         for ( UINT32 i = 0 ; i < _eleSize ; ++i )
         {
            if ( !(_staticBuf[ i ] < val) )
            {
               return iterator( &_staticBuf[ i ], _staticBuf, &_eleSize ) ;
            }
         }
         return end() ;
      }

      OSS_INLINE iterator upper_bound( const T& val ) const
      {
         if ( _pSet )
         {
            return iterator( _pSet->upper_bound( val ) ) ;
         }
         for ( UINT32 i = 0 ; i < _eleSize ; ++i )
         {
            if ( val < _staticBuf[ i ] )
            {
               return iterator( &_staticBuf[ i ], _staticBuf, &_eleSize ) ;
            }
         }
         return end() ;
      }

      OSS_INLINE pair<iterator, iterator> equal_range( const T& val ) const
      {
         if ( _pSet )
         {
            pair< typename ossPoolSet<T>::iterator, typename ossPoolSet<T>::iterator > tmp =
               _pSet->equal_range( val ) ;
            return pair<iterator, iterator>( iterator( tmp.first ),
                                             iterator( tmp.second ) ) ;
         }
         else
         {
            iterator itBegin = end() ;
            iterator itEnd = end() ;
            for ( UINT32 i = 0 ; i < _eleSize ; ++i )
            {
               if ( !( _staticBuf[ i ] < val ) &&
                    !( val < _staticBuf[ i ] ) )
               {
                  if ( itBegin == end() )
                  {
                     itBegin = iterator( &_staticBuf[ i ], _staticBuf,
                                         &_eleSize ) ;
                  }
               }
               else if ( itBegin() != end() ) /// has set the begin
               {
                  itEnd = iterator( &_staticBuf[ i ], _staticBuf, &_eleSize ) ;
                  break ;
               }
            }
            return pair<iterator, iterator>( itBegin, itEnd ) ;
         }
      }

      OSS_INLINE void shift2Stack( UINT32 divisor = 8 )
      {
         if ( _pSet )
         {
            UINT32 threshold = 0 ;
            if ( divisor > 0 )
            {
               threshold = stackSize / divisor ;
            }
            if ( _pSet->size() <= threshold )
            {
               /// copy data to stack
               _eleSize = 0 ;
               typename ossPoolSet<T>::iterator it = _pSet->begin() ;
               for ( ; it != _pSet->end() ; ++it )
               {
                  _staticBuf[ _eleSize++ ] = *it ;
               }
               /// release the deque
               delete _pSet ;
               _pSet = NULL ;
            }
         }
      }

   protected:
      OSS_INLINE UINT32 _findInStackBuf( const T& value ) const
      {
         UINT32 i = 0 ;
         while( i < _eleSize )
         {
            if ( !( _staticBuf[ i ] < value ) &&
                 !( value < _staticBuf[ i ] ) )
            {
               return i ;
            }
            ++i ;
         }
         return this->npos ;
      }

      OSS_INLINE UINT32 _capacity() const
      {
         if ( _pSet )
         {
            return _pSet->size() ;
         }
         return stackSize ;
      }

      OSS_INLINE INT32 _ensureSpace( UINT32 size )
      {
         INT32 rc = SDB_OK ;

         if ( !_pSet && size > stackSize )
         {
            _pSet = new (std::nothrow) ossPoolSet<T> ;
            if ( !_pSet )
            {
               rc = SDB_OOM ;
               goto error ;
            }
            /// copy stack data to deque
            for ( UINT32 i = 0 ; i < _eleSize ; ++i )
            {
               _pSet->insert( _staticBuf[ i ] ) ;
               _staticBuf[ i ] = T() ;
            }
            _eleSize = 0 ;
         }

      done:
         return rc ;
      error:
         goto done ;
      }

   private:
      T              _staticBuf[ stackSize ] ;
      ossPoolSet<T>* _pSet ;
      UINT32         _eleSize ;

   public:
      const static UINT32  npos ;

   } ;

   template< typename T, UINT32 stackSize >
   const UINT32 _utilSet<T,stackSize>::npos = ~0 ;

}

#endif // UTIL_SET_HPP_

