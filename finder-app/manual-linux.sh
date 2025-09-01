#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

# OUTDIR=/tmp/aeld
OUTDIR=/home/nkarippa/UCB/as3_outdir
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
PROC_COUNT=$(($(nproc)-2)) #for integer evaluation

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

if [ ! -d "${OUTDIR}" ]; then
    mkdir -p ${OUTDIR}
fi

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
    make -j${PROC_COUNT} ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all
fi

echo "Adding the Image in outdir"
if [ ! -d "${OUTDIR}/linux-stable/arch/arm64/boot/Image" ]; then
    cp ${OUTDIR}/linux-stable/arch/arm64/boot/Image ${OUTDIR}/
fi


# PART 2

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    rm -rf ${OUTDIR}/rootfs
fi

mkdir -p ${OUTDIR}/rootfs
cd rootfs
# TODO: Create necessary base directories so that the skeleton is ready for busybox
# PART 3
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log


echo "Building busybox..."
cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
else
    cd busybox
fi

# TODO: Make and install busybox
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} distclean
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CONFIG_PREFIX=${OUTDIR}/rootfs install

# PART 4

# echo "Library dependencies"
cd "$OUTDIR"

${CROSS_COMPILE}readelf -a rootfs/bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a rootfs/bin/busybox | grep "Shared library"
toolchain_path=$(which ${CROSS_COMPILE}readelf)
toolchain_root=$(dirname ${toolchain_path})
toolchain_root=$(dirname ${toolchain_root})

# TODO: Add library dependencies to rootfs
# libm.so.6
# libresolv.so.2
# libc.so.6
# ld-linux-aarch64.so.1


libpath=$(find ${toolchain_root} -name libm.so.6)
echo ${libpath}
cp ${libpath} rootfs/lib64
libpath=$(find ${toolchain_root} -name libresolv.so.2)
echo ${libpath}
cp ${libpath} rootfs/lib64
libpath=$(find ${toolchain_root} -name libc.so.6)
echo ${libpath}
cp ${libpath} rootfs/lib64
libpath=$(find ${toolchain_root} -name ld-linux-aarch64.so.1)
echo ${libpath}
cp ${libpath} rootfs/lib


cd "$OUTDIR"/rootfs

# TODO: Make device nodes
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# PART 5

# TODO: Clean and build the writer utility
cd "${FINDER_APP_DIR}"
make clean
make CROSS_COMPILE=${CROSS_COMPILE} 

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs

# Copy your finder.sh, conf/username.txt, conf/assignment.txt and finder-test.sh scripts from Assignment 2 into the outdir/rootfs/home directory.
#  i. Modify the finder-test.sh script to reference conf/assignment.txt instead of ../conf/assignment.txt.
cp writer finder.sh finder-test.sh autorun-qemu.sh ${OUTDIR}/rootfs/home
mkdir -p ${OUTDIR}/rootfs/home/conf
cp conf/username.txt conf/assignment.txt ${OUTDIR}/rootfs/home/conf

# TODO: Chown the root directory
cd "$OUTDIR"
cd rootfs/
sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
find . | cpio -H newc -ov --owner root:root > initramfs.cpio
gzip -f initramfs.cpio 
mv initramfs.cpio.gz ../