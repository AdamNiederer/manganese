rm -rf /tmp/manganese-iso /tmp/manganese-fs

mkdir -p /tmp/manganese-iso/cde/optional /tmp/manganese-fs

xorriso -osirrox on -indev tinycore.iso -extract / /tmp/manganese-iso

pushd /tmp/manganese-fs
cat /tmp/manganese-iso/boot/corepure64.gz | gunzip | cpio -i -H newc -d
popd

strip manganese
cp manganese /tmp/manganese-fs/bin
ln -s lib /tmp/manganese-fs/lib64
mkdir -p /tmp/manganese-fs/home/tc /tmp/manganese-fs/usr/share/licenses/OpenBLAS /tmp/manganese-fs/usr/share/licenses/SIMDxorshift
mkdir -p
cp OpenBLAS/LICENSE /tmp/manganese-fs/usr/share/licenses/OpenBLAS
cp SIMDxorshift/LICENSE /tmp/manganese-fs/usr/share/licenses/SIMDxorshift
echo -e "\necho -e 'Manganese is also distributed with ABSOLUTELY NO WARRANTY.\nhttps://github.com/AdamNiederer/manganese\n'\necho 'Loading dependencies... (waiting for network to settle)'\nsleep 4\ntce-load -iw gcc_libs\nsudo manganese 98%" >> /tmp/manganese-fs/home/tc/.profile
sed -i -e 's/prompt 1/prompt 0/g' /tmp/manganese-iso/boot/isolinux/isolinux.cfg

pushd /tmp/manganese-fs
find . | cpio -H newc -o | gzip -9 > /tmp/manganese-iso/boot/corepure64.gz
popd

xorriso -as mkisofs -iso-level 3 -full-iso9660-filenames -volid "manganese" \
        -eltorito-boot boot/isolinux/isolinux.bin -boot-load-size 4 \
        -eltorito-catalog boot/isolinux/boot.cat -boot-info-table \
        -no-emul-boot -output "manganese.iso" /tmp/manganese-iso
