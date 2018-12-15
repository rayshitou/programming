package main

import "fmt"

const (
	_ 	= iota
	KB float64 = 1 << (10 * iota)
	MB 
	GB 
	TB 
	PB 
	EB
	ZB
	YB
)

func main() {
	fmt.Println(ZB)
	fmt.Println(GB)
	fmt.Println(MB)
	fmt.Println(KB)
}
