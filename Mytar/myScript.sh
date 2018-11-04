if [[ -e mytar.o  ]]; then
  if [[ -d tmp ]]; then
      rm -r tmp
    #statements
  fi
  mkdir tmp
  cd tmp
  echo 'Hello world.' >file1.txt
  head  /etc/passwd >file2.txt
  head -c 1024 /dev/urandom>text3.dat
  cd ..
  ./mytar -cf filetar.mtar  /tmp/file1.txt  /tmp/file2.txt  /tmp/file3.dat
  mkdir out

 	cp filetar.mtar ./out
 	cd out
 	../mytar -xf filetar.mtar
  if test -n "$(diff ../tmp/file1.txt file1.txt 2>&1)" ; then
 		echo "File file1 is not exactly equal"
    cd ../..
 		exit -1
  fi
 	if test -n "$(diff ../tmp/file2.txt file2.txt 2>&1)" ; then
 		echo "File file2 is not exactly equal"
    cd ../..
 		exit -1
  fi
 	if test -n "$(diff ../tmp/file3.dat file3.dat 2>&1)" ; then
 		echo "File file3 is not exactly equal"
    cd ../..
 		exit -1
  fi

    cd ../..
 		echo "Success"
 		exit 0




else printf %s "error"
     exit 0
fi
