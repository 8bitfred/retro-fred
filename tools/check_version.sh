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

version_count=$(grep '^# ' CHANGELOG.md | wc -l)
last_version=$(grep -oP '^# \K\S*' CHANGELOG.md | head -1)
cmake_version=$(grep -oP '^project.*VERSION\s+\K[\d\.]*' CMakeLists.txt)
android_version_code=$(grep -oP 'android:versionCode="\K[^"]+' android/app/src/main/AndroidManifest.xml)
android_version_name=$(grep -oP 'android:versionName="\K[^"]+' android/app/src/main/AndroidManifest.xml)

if [ "$version_count" != "$android_version_code" ]; then
    echo -n "::error title='Mismatching Android version code'::"
    echo "Android version code ($android_version_code) does not match number of releases in CHANGELOG.md ($version_count)"
    exit 1
fi
if [ "$last_version" != "$android_version_name" ]; then
    echo -n "::error title='Mismatching Android version name'::"
    echo "Andoird version name ($android_version_name) does not match latest release in CHANGELOG.md ($last_version)"
    exit 1
fi
if [ "$last_version" != "$cmake_version" ]; then
    echo -n "::error title='Mismatching Android version name'::"
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
if [ -n "$SHOW_VERSION" ]; then
    echo "$cmake_version"
fi
