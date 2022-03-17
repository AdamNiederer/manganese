rm -rf iso fs

mkdir -p iso fs

xorriso -osirrox on -indev ./CorePure64-13.0.iso -extract / iso

cd fs
cat ../iso/boot/corepure64.gz | gunzip | cpio -i -H newc -d
cd ..

strip manganese
cp manganese fs/bin
ln -s lib fs/lib64
cp /lib/libgomp.so* fs/lib
cp /lib/libm.so* fs/lib
cp /lib/libgfortran.so* fs/lib
cp /lib/libquadmath.so* fs/lib
cp /lib/libgcc_s.so* fs/lib
cp /lib/libopenblas.so* fs/lib
echo "manganese 96%" >> fs/opt/bootlocal.sh
sed -i -e 's/prompt 1/prompt 0/g' iso/boot/isolinux/isolinux.cfg

cd fs
find . | cpio -H newc -o | gzip -9 > ../iso/boot/corepure64.gz
cd ..

xorriso -as mkisofs -iso-level 3 -full-iso9660-filenames -volid "manganese" \
        -eltorito-boot boot/isolinux/isolinux.bin -boot-load-size 4 \
        -eltorito-catalog boot/isolinux/boot.cat -boot-info-table \
        -no-emul-boot -output "manganese.iso" iso
