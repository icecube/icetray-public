#!/bin/sh

# This Script will upload the documnentation created by docs-build to the 
# documentation server (http://docs.icecube.aq).
# First it creates a tarball then it uses curl to access the REST interface on the server

if [ -z "${ICECUBE_PASSWORD}" ]; then
    echo ICECUBE_PASSWORD not set, aborting
    exit -1
fi

GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`
GIT_REVISION=`git rev-parse --verify --short HEAD`
TARBALL_NAME="${I3_BUILD}/docs/icetray.${GIT_BRANCH}.${GIT_REVISION}.documentation.tar.gz"

TARCMD="tar -czvf $TARBALL_NAME -C${I3_BUILD}/docs/build/html ."
echo running $TARCMD
${TARCMD}

UPLOADCMD="curl -XPUT -i --data-binary @${TARBALL_NAME} https://docs.icecube.aq/api/upload?path=icetray/main -u icecube:${ICECUBE_PASSWORD}"
echo running ${UPLOADCMD}
${UPLOADCMD}
