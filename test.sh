#! /bin/bash

if [[ $2 = 'm' ]]
then
    make re -C philo_$1
fi

echo no args
./philo_$1/philo_$1
echo

echo "5 800 200 (too few args)"
./philo_$1/philo_$1 5 800 200
echo

echo "5 800 200 200 3 3 (too many args)"
./philo_$1/philo_$1 5 800 200 200 3 3
echo

echo "5 800 200 200 t (non-number arg)"
./philo_$1/philo_$1 5 800 200 200 t
echo

echo "5 800 200 200 3 t (non-number arg)"
./philo_$1/philo_$1 5 800 200 200 3 t
echo

# echo "5 800 2t0 200 (non-number arg)"
# ./philo_$1/philo_$1 5 800 2t0 200
# echo

echo "t (non-number arg)"
./philo_$1/philo_$1 t
echo