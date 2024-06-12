#!/bin/bash

# fail if any command fails
set -e

SOURCE_DIR=$(realpath $(dirname "$0")/..)
cd "$SOURCE_DIR"

if [ "$1" = "--check-release-ref" ]; then
    CHECK_RELEASE_REF="$2"
elif [ "$1" = "--check-unique" ]; then
    CHECK_UNIQUE="$2"
elif [ "$1" == "--show-version" ]; then
    SHOW_VERSION=1
fi

last_version=$(grep -oP '^# \K\S*' CHANGELOG.md | head -1)
cmake_version=$(grep -oP '^project.*VERSION\s+\K[\d\.]*' CMakeLists.txt)
android_version_code=$(grep -oP "^$cmake_version"'\s+\K\S+' android/versions)
manifest_version_code=$(grep -oP 'android:versionCode="\K[^"]+' android/app/src/main/AndroidManifest.xml)
manifest_version_name=$(grep -oP 'android:versionName="\K[^"]+' android/app/src/main/AndroidManifest.xml)
gradle_version_code=$(grep -oP 'versionCode\s+\K\S+' android/app/build.gradle)
gradle_version_name=$(grep -oP 'versionName\s+"\K[^"]+' android/app/build.gradle)

if [ -n "$SHOW_VERSION" ]; then
    echo "$cmake_version"
else
    echo "Version information:"
    echo "    Last version in CHANGELOG.md: $last_version"
    echo "    Version name in CMakeLists.txt: $cmake_version"
    echo "    Version code for $cmake_version in android/versions: $android_version_code"
    echo "    Version code in AndroidManifest.xml: $manifest_version_code"
    echo "    Version name in AndroidManifest.xml: $manifest_version_name"
    echo "    Version code in build.gradle: $gradle_version_code"
    echo "    Version name in build.gradle: $gradle_version_name"
fi

if [ -z "$android_version_code" ]; then
    echo -n "::error title='Unknown Android version name'::"
    echo "Version name $cmake_version not found in android/versions table"
    exit 1
fi
if [ "$manifest_version_code" != "$android_version_code" ]; then
    echo -n "::error title='Mismatching Android manifest version code'::"
    echo -n "Android version code ($android_version_code) does not match version code in "
    echo "AndroidManifest.xml ($manifest_version_code)"
    exit 1
fi
if [ "$manifest_version_name" != "$cmake_version" ]; then
    echo -n "::error title='Mismatching Android manifest version name'::"
    echo -n "CMake version ($cmake_version) does not match version name in "
    echo "AndroidManifest.xml ($manifest_version_name)"
    exit 1
fi
if [ "$gradle_version_code" != "$android_version_code" ]; then
    echo -n "::error title='Mismatching Gradle build version code'::"
    echo -n "Android version code ($android_version_code) does not match "
    echo "version code in build.gradle ($gradle_version_code)"
    exit 1
fi
if [ "$gradle_version_name" != "$cmake_version" ]; then
    echo -n "::error title='Mismatching Gradle build version name'::"
    echo "CMake version ($cmake_version) does not match version name in build.gradle ($gradle_version_name)"
    exit 1
fi
if [ "$last_version" != "$cmake_version" ]; then
    echo -n "::error title='Mismatching version name'::"
    echo "CMake version ($cmake_version) does not match latest release in CHANGELOG.md ($last_version)"
    exit 1
fi
if [ -n "$CHECK_RELEASE_REF" ]; then
    if [ "$CHECK_RELEASE_REF" != "refs/tags/${cmake_version}" ]; then
        echo -n "::error title='Mismatching tag name'::"
        echo "Tag ref ($CHECK_RELEASE_REF) does not match version number ($cmake_version)"
        exit 1
    fi
fi
if [ -n "$CHECK_UNIQUE" ]; then
    if gh api \
        -H "Accept: application/vnd.github+json" \
        -H "X-GitHub-Api-Version: 2022-11-28" \
        "/repos/${CHECK_UNIQUE}/git/ref/tags/${cmake_version}"; then
        echo -n "::error title='Tag already exists'::"
        echo "Tag ${cmake_version} already exists in repository"
        exit 1
    fi
fi
