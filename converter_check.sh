function compare {
  r1=$(wtk-firealarm $1 $3 $4)
  r2=$(wtk-firealarm $2 $3 $4)
  # echo ${r1:7:4} ${r2:7:4}
  #echo -e '\n'
  if [[ "${r1:7:4}" != "${r2:7:4}" ]]; then
    echo -e "FAIL!" $1 $3 $4
  fi
}
./IR120.sh example/add4/add4.rel a.rel
./IR120.sh example/add4/add4_anon.rel b.rel
./IR120.sh example/add4/add4_func.rel c.rel
./IR120.sh example/add4/add4_check.rel d.rel
for ins in example/two-bit/*.ins
do
	for wit in example/two-bit/*.wit
	do
		compare example/add4/add4.rel a.rel $ins $wit
		compare example/add4/add4_anon.rel b.rel $ins $wit
		compare example/add4/add4_func.rel c.rel $ins $wit
		compare example/add4/add4_check.rel d.rel $ins $wit
   done
done


./IR120.sh example/add4_or_mul4/add4_or_mul4.rel a.rel
./IR120.sh example/add4_or_mul4/add4_or_mul4_anon.rel b.rel
./IR120.sh example/add4_or_mul4/add4_or_mul4_input.rel c.rel
./IR120.sh example/add4_or_mul4/add4_or_mul4_input_anon.rel d.rel
./IR120.sh example/unbalanced2.rel e.rel
for ins in example/two-bit/*.ins
do
	for wit in example/three-bit/*wit
	do
		compare example/add4_or_mul4/add4_or_mul4.rel a.rel $ins $wit
		compare example/add4_or_mul4/add4_or_mul4_anon.rel b.rel $ins $wit
		compare example/add4_or_mul4/add4_or_mul4_input.rel c.rel $ins $wit
		compare example/add4_or_mul4/add4_or_mul4_input_anon.rel d.rel $ins $wit
		compare example/unbalanced2.rel e.rel $ins $wit
   done
done

./IR120.sh example/unbalanced.rel a.rel
for ins in example/three-bit/*.ins
do
	for wit in example/three-bit/*wit
	do
		compare example/unbalanced.rel a.rel $ins $wit
   done
done


./IR120.sh example_arithmetic/add4/add4.rel a.rel
./IR120.sh example_arithmetic/add4/add4_anon.rel b.rel
./IR120.sh example_arithmetic/add4/add4_func.rel c.rel
./IR120.sh example_arithmetic/add4/add4_check.rel d.rel
for ins in example_arithmetic/add4/*.ins
do
	for wit in example_arithmetic/add4/*.wit
	do
		compare example_arithmetic/add4/add4.rel a.rel $ins $wit
		compare example_arithmetic/add4/add4_anon.rel b.rel $ins $wit
		compare example_arithmetic/add4/add4_func.rel c.rel $ins $wit
		compare example_arithmetic/add4/add4_check.rel d.rel $ins $wit
   done
done

./IR120.sh example_arithmetic/add4_or_mul4/add4_or_mul4.rel a.rel
./IR120.sh example_arithmetic/add4_or_mul4/add4_or_mul4_anon.rel b.rel
./IR120.sh example_arithmetic/add4_or_mul4/add4_or_mul4_input.rel c.rel
./IR120.sh example_arithmetic/add4_or_mul4/add4_or_mul4_input_anon.rel d.rel
for ins in example_arithmetic/add4_or_mul4/*.ins
do
	for wit in example_arithmetic/add4_or_mul4/*.wit
	do
		compare example_arithmetic/add4_or_mul4/add4_or_mul4.rel a.rel $ins $wit
		compare example_arithmetic/add4_or_mul4/add4_or_mul4_anon.rel b.rel $ins $wit
		compare example_arithmetic/add4_or_mul4/add4_or_mul4_input.rel c.rel $ins $wit
		compare example_arithmetic/add4_or_mul4/add4_or_mul4_input_anon.rel d.rel $ins $wit
   done
done

./IR120.sh example_arithmetic/sum_test/sum_test.rel a.rel
compare example_arithmetic/sum_test/sum_test.rel a.rel example_arithmetic/sum_test/sum_test.ins example_arithmetic/sum_test/sum_test.wit

for n in 10
do
  ./IR120.sh example/sha256x$n.rel a.rel
  compare example/sha256x$n.rel a.rel example/sha.ins example/sha.wit
done



for n in 5 10 100
do
  ./IR120.sh example_arithmetic/matprod$n.rel a.rel
  compare example_arithmetic/matprod$n.rel a.rel example_arithmetic/matprod$n.ins example_arithmetic/matprod$n.wit
done


rm a.rel b.rel c.rel d.rel e.rel
