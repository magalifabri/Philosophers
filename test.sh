#! /bin/bash

if [[ $1 = 'm' ]]
then
    make re -C philo_three
fi

echo no args
./philo_three/philo_three
echo

echo "5 800 200 (too few args)"
./philo_three/philo_three 5 800 200
echo

echo "5 800 200 200 3 3 (too many args)"
./philo_three/philo_three 5 800 200 200 3 3
echo

echo "5 800 200 200 t (non-number arg)"
./philo_three/philo_three 5 800 200 200 t
echo

echo "5 800 200 200 3 t (non-number arg)"
./philo_three/philo_three 5 800 200 200 3 t
echo

# echo "5 800 2t0 200 (non-number arg)"
# ./philo_three/philo_three 5 800 2t0 200
# echo

echo "t (non-number arg)"
./philo_three/philo_three t
echo