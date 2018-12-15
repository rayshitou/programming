package main

import (
	"fmt"
	"runtime"
)

func f(left, right chan int) {
	left <- 1 + <- right
}

func main() {
	const n = 100
	leftmost := make(chan int)
	left := leftmost
	right := make(chan int)
	for i := 0; i < n; i++ {
		right = make(chan int)
		go f(left, right)
		left = right
	}
	go func(c chan int) { c <- 1} (right)
	fmt.Println(<-leftmost)

	fmt.Println(runtime.NumCPU())
	fmt.Println(runtime.GOMAXPROCS(0))
}
