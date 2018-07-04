#!/bin/sh

SUCCESS="01 02 03 04 05 06 10 11 13 14 15 16 17 18 19 20 21 22 23 24 26 27 28 29 30 31 34 35 39 40 41 44 45 46 47 48 50 51 52 53 54 57 59 60 62 63 64 65 66 67 68 69 70 75 76 79 80 81 82 83"
FAIL="07 08 09 12 25 32 33 36 37 38 42 43 49 55 56 58 61 71 72 73 74 77 78 84 85 86 87"
C="01 02 04 06 09 10 18 21 27 29 30 31 34 45 46 52 54 67 68 70 71 75 76 79 81 82 83 87"
T="05 08 11 13 14 19 23 25 32 35 36 39 41 42 44 47 48 49 50 57 61 63 64 65 66 69 73 74 77 80"
TS="03 07 12 15 16 17 20 22 24 26 28 33 37 38 40 43 51 53 55 56 58 59 60 62 72 78 84 85 86"

SS=""
for i in $SUCCESS
do
	SS="$SS logs/S"$i"_*.txt"
done
FF=""
for i in $FAIL
do
	FF="$FF logs/S"$i"_*.txt"
done

./convert-to-csv.prl $SS > logs_SUCCESS.csv
./convert-to-csv.prl $FF > logs_FAIL.csv

AAC=""
for i in $C
do
	AAC="$AAC logs/S"$i"_*.txt"
done
AAT=""
for i in $T
do
	AAT="$AAT logs/S"$i"_*.txt"
done
AATS=""
for i in $TS
do
	AATS="$AATS logs/S"$i"_*.txt"
done

./convert-to-csv.prl $AAC > logs_C.csv
./convert-to-csv.prl $AAT > logs_M.csv
./convert-to-csv.prl $AATS > logs_S.csv

