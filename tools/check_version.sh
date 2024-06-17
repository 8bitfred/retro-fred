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
manifest_version_code=$(grep -oP 'android:versionCode="\K[^"]+' android/app/src/main/AndroidManifest.xml)
manifest_version_name=$(grep -oP 'android:versionName="\K[^"]+' android/app/src/main/AndroidManifest.xml)
gradle_version_code=$(grep -oP 'versionCode\s+\K\S+' android/app/build.gradle)
gradle_version_name=$(grep -oP 'versionName\s+"\K[^"]+' android/app/build.gradle)
android_version_name=$(grep -oP '\S+(?=\s+'$gradle_version_code'\b)' android/versions)

if [ -n "$SHOW_VERSION" ]; then
    echo "$cmake_version"
else
    echo "Version information:"
    echo "    Last version in CHANGELOG.md: $last_version"
    echo "    Version name in CMakeLists.txt: $cmake_version"
    echo "    Version code in AndroidManifest.xml: $manifest_version_code"
    echo "    Version name in AndroidManifest.xml: $manifest_version_name"
    echo "    Version code in build.gradle: $gradle_version_code"
    echo "    Version name in build.gradle: $gradle_version_name"
    echo "    Version name for version code $gradle_version_code in android/versions: $android_version_name"
fi

if [ -z "$android_version_name" ]; then
    echo -n "::error title='Unknown Android version name'::"
    echo "Version code $gradle_version_code not found in android/versions table"
    exit 1
fi
if [ "$manifest_version_name" != "$cmake_version" ]; then
    echo -n "::error title='Mismatching Android manifest version name'::"
    echo -n "CMake version ($cmake_version) does not match version name in "
    echo "AndroidManifest.xml ($manifest_version_name)"
    exit 1
fi
if [ "$manifest_version_name" != "$android_version_name" ]; then
    echo -n "::error title='Mismatching Android manifest version name'::"
    echo -n "Android version name ($android_version_name) does not match version name in "
    echo "AndroidManifest.xml ($manifest_version_name)"
    exit 1
fi
if [ "$gradle_version_name" != "$cmake_version" ]; then
    echo -n "::error title='Mismatching Gradle build version name'::"
    echo "CMake version ($cmake_version) does not match version name in build.gradle ($gradle_version_name)"
    exit 1
fi
if [ "$gradle_version_name" != "$android_version_name" ]; then
    echo -n "::error title='Mismatching Gradle build version name'::"
    echo -n "Android version name ($android_version_name) does not match "
    echo "version name in build.gradle ($gradle_version_name)"
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
