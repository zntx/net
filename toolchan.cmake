SET(CMAKE_SYSTEM_NAME Linux)

# #指定C交叉编译噿,必须配置
# #或交叉编译器使用绝忹地址
SET(CMAKE_C_COMPILER "arm-himix200-linux-gcc")

# #指定C++交叉编译噿
SET(CMAKE_CXX_COMPILER "arm-himix200-linux-g++")

SET(CMAKE_FIND_ROOT_PATH  /opt/hisi-linux/x86-arm/arm-himix200-linux/arm-linux-gnuabi )
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(LWS_OPENSSL_INCLUDE_DIRS /data1/zhangyuzhu8/nfs/install/include )
set(LWS_OPENSSL_LIBRARIES "/data1/zhangyuzhu8/nfs/install/lib/libssl.so;/data1/zhangyuzhu8/nfs/install/lib/libcrypto.so")
set(ZLIB_LIBRARY /data1/zhangyuzhu8/work/NP_FVW_V2.2.0/apps/libs/libs_ezviz/)
set(ZLIB_INCLUDE_DIR /data1/zhangyuzhu8/work/NP_FVW_V2.2.0/apps/modules/mtd-utils-1.0.0/)
set(OPENSSL_CRYPTO_LIBRARY /data1/zhangyuzhu8/work/NP_FVW_V2.2.0/apps/libs/libs_hisi/)
set(OPENSSL_INCLUDE_DIR /data1/zhangyuzhu8/work/NP_FVW_V2.2.0/apps/app/include/)
set(OPENSSL_SSL_LIBRARY /data1/zhangyuzhu8/work/NP_FVW_V2.2.0/apps/libs/libs_hisi/)


