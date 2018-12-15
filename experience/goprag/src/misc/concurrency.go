
package main

import (
	"fmt"
	"time"
	"math/rand"
)

func boring(msg string) <- chan string {
	c := make(chan string)
	go func() {
		for i := 0; i<9 ; i++ {
			c <- fmt.Sprintf("%s, %d", msg, i)
			time.Sleep(time.Duration(rand.Intn(1e3)) * time.Millisecond)
		}
	}()
	return c
}

func fanin(in1, in2 <-chan string) <-chan string {
	c := make(chan string)
	go func(){
		for{
			select{
			case s := <-in1: c<-s
			case s := <-in2: c<-s
			}
		}}()
	return c
}

type Message struct {
	str string
	wait chan bool
}

func main() {
	//c := fanin(boring("Boring!"),boring("relax"))
	c := boring("jan")
	//timeout := time.After(10*time.Second)
	for {
	timeout := time.After(1*time.Second)
		select {
		case  s := <-c:
			fmt.Println(s)
		case <- timeout:
			fmt.Println("you're so slow")
			return
		}
	}
/*	for i := 0; i<18; i++ {
		fmt.Println( <-c)
	}*/
}
