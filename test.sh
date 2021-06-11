#! /bin/bash

if [[ $1 = 1 ]]
then
    PHILO="philo_one"
elif [[ $1 = 2 ]]
then
    PHILO="philo_two"
elif [[ $1 = 3 ]]
then
    PHILO="philo_three"
else
    echo give one argument: 1, 2 or 3
    exit 0
fi

make -C $PHILO
MAKE_RET=$?
if [[ $MAKE_RET != 0 ]]
then
    exit 1
fi

# bad number of args
$PHILO/$PHILO
$PHILO/$PHILO 4
$PHILO/$PHILO 4 410
$PHILO/$PHILO 4 410 200
$PHILO/$PHILO 4 410 200 200 3 3

# bad values
$PHILO/$PHILO 301 410 200 200 3
$PHILO/$PHILO 1 410 200 200 3
$PHILO/$PHILO 0 410 200 200 3
$PHILO/$PHILO -1 410 200 200 3

$PHILO/$PHILO 4 0 200 200 3
$PHILO/$PHILO 4 -1 200 200 3

$PHILO/$PHILO 4 410 0 200 3
$PHILO/$PHILO 4 410 -1 200 3

$PHILO/$PHILO 4 410 200 0 3
$PHILO/$PHILO 4 410 200 -1 3

$PHILO/$PHILO 4 410 200 200 0
$PHILO/$PHILO 4 410 200 200 -1

# $PHILO/$PHILO 4 410 200 200 3