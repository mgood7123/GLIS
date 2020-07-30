set -x
cd corrade
abi="armeabi-v7a"
arch="arm"
api="28"
ndk="$HOME/Android/Sdk/ndk/21.1.6352462"
host="linux-x86_64"
frp1="$ndk/platforms/android-$api/arch-$arch"
frp2="$ndk/toolchains/llvm/prebuilt/$host/sysroot"
ip="$frp1/usr"
iip="$frp2/usr"

mkdir "build-android-$arch"
cd "build-android-$arch"

cmake .. \
    -DCMAKE_ANDROID_NDK="$ndk" \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION="$api" \
    -DCMAKE_ANDROID_ARCH_ABI="$abi" \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$ip" \
    -DCMAKE_FIND_ROOT_PATH="$frp1;$frp2" \
    -DCORRADE_INCLUDE_INSTALL_PREFIX="$iip" \
    -DCORRADE_RC_EXECUTABLE=$(which corrade-rc) \
    -DWITH_RC=ON
cmake --build .
cmake --build . --target install
cd ..
rm -rf "build-android-$arch"

