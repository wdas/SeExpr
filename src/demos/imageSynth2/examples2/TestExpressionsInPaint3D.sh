#!/bin/bash
#
#  Usage: ./TestExpressionsInPaint3D.sh <expression file root dir> <output dir>
#  Output: one file called "result". One directory hierarchy same as <expression file root dir>
#

if [ "$#" -ne 2 ]; then
    echo "first arg is sefile root dir. second arg is output dir."
    exit 1
fi

if [ ! -d $1 ]; then
    echo "need sefile directory as the first argument"
    exit 1
fi

if [ ! -d $2 ]; then
    echo "need output directory as the second argument"
    exit 1
fi

allowNoVarOrNoDef() {
     if grep -q "No variable named" "$1"; then
	       echo > /dev/null
     elif grep -q "No function named" "$1"; then
           echo > /dev/null
     elif grep -q "has no definition" "$1"; then
           echo > /dev/null
     elif grep -q "Syntax error" "$1"; then
           echo > /dev/null
     else
        echo "Not no var or no def or syntax problem"
        exit 1;
     fi
}

doit() {

  sefile_root=$1
  remove_prefix="$(dirname $1)"
  plugin_dir=/disney/users/yuanfanc/projects/seexpr/seexprplugins/Linux-2.6.32-x86_64-debug/plugins/se
  install_dir=../../../../
  retdir=$2

  sefilecopy=$retdir/${3#$remove_prefix}
  errorfile=${sefilecopy}.error
  outputfile1=${sefilecopy}.interp
  outputpng1=${sefilecopy}.interp.png
  outputfile2=${sefilecopy}.codegen
  outputpng2=${sefilecopy}.codegen.png
  
  # show progress
  echo $3
  
  # if already done, skip
  if [ -f "$outputfile1" ] && [ -f "$outputfile2" ]
  then
  	  return
  fi

  curdir=$(dirname "$sefilecopy")
  mkdir -p $curdir

  success=true
  echo $3 >> "$errorfile"
  
  # TODO: not portable? work for csh only?
  env SE_EXPR_PLUGINS="$plugin_dir" ${install_dir}/bin/imageSynthPaint3d "$outputpng1" 200 200 "$3" >& "$outputfile1"
  if [ $? -ne 0 ]; then
     echo "interpreter fail" >> "$errorfile"
     success=false;
     allowNoVarOrNoDef "$outputfile1"
  fi

  env SE_EXPR_PLUGINS="$plugin_dir" ${install_dir}/bin/imageSynthLLVMPaint3d "$outputpng2" 200 200 "$3" >& "$outputfile2"
  if [ $? -ne 0 ]; then
     echo "codegen fail" >> "$errorfile"
     success=false;
     allowNoVarOrNoDef "$outputfile2"
  fi

  if [ $success = false ] ; then
     cp "$3" "$curdir"
     echo >> "$errorfile"
     return
  fi

  if cmp -s "$outputpng1" "$outputpng2" ; then
     rm "$errorfile" "$outputpng1" "$outputpng2"
  else
     echo "byte-to-byte comparison fail, try image comparison..." >> "$errorfile"
     if [ -n $(idiff -a "$outputpng1" "$outputpng2" | grep PASS) ]; then
        rm "$errorfile" "$outputpng1" "$outputpng2"
     else
        cp "$3" "$curdir"
     fi
  fi
}

cpus=$(getconf _NPROCESSORS_ONLN)
export -f allowNoVarOrNoDef
export -f doit
find "$1" -name "*.se" -print0 | xargs -0 -n 1 -P $cpus -i bash -c 'doit "$@"' _ "$1" "$2" {}

find "$2" -name "*.error" | while read errfile
do
    cat "$errfile" >> "$2"/failed_sefile
    rm "$errfile"
done
