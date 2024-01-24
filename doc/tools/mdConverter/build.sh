#!/bin/sh

sourcePath="./src"

g++ -o build/mdConverter.o ${sourcePath}/mdConverter.cpp -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

g++ -o build/iniReader.o ${sourcePath}/iniReader.cpp -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

g++ -o build/mdParser.o ${sourcePath}/mdParser.cpp -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

g++ -o build/system.o ${sourcePath}/system.cpp -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

g++ -o build/options.o ${sourcePath}/options.cpp -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/ini.o ${sourcePath}/inih/ini.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/cJSON2.o ${sourcePath}/json/cJSON2.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/cJSON_iterator.o ${sourcePath}/json/cJSON_iterator.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/oss.o ${sourcePath}/oss/oss.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/ossMem.o ${sourcePath}/oss/ossMem.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/autolink.o ${sourcePath}/sundown/autolink.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/buffer.o ${sourcePath}/sundown/buffer.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/houdini_href_e.o ${sourcePath}/sundown/houdini_href_e.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/houdini_html_e.o ${sourcePath}/sundown/houdini_html_e.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/html.o ${sourcePath}/sundown/html.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/html_smartypants.o ${sourcePath}/sundown/html_smartypants.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/markdown.o ${sourcePath}/sundown/markdown.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

gcc -o build/stack.o ${sourcePath}/sundown/stack.c -I${sourcePath}/json -I${sourcePath}/include -c -fPIC -fno-strict-aliasing -ggdb -pthread -Wno-write-strings -Wall -Wsign-compare -Wno-unknown-pragmas -Winvalid-pch -Wno-address -pipe -fno-builtin-memcmp -O0 -fstack-protector -D_DEBUG -D__STDC_LIMIT_MACROS -DHAVE_CONFIG_H -DBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC -DSDB_DLL_BUILD -D_GNU_SOURCE -DXP_UNIX -D_FILE_OFFSET_BITS=64

g++ build/stack.o build/markdown.o build/html_smartypants.o build/html.o build/houdini_html_e.o build/houdini_href_e.o build/mdConverter.o build/iniReader.o build/mdParser.o build/system.o build/options.o build/ini.o build/cJSON2.o build/cJSON_iterator.o build/oss.o build/ossMem.o build/autolink.o build/buffer.o -o build/linux_mdConverter