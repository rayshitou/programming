package main

import (
	"fmt"
	"time"
	"heap"
)

type Request struct {
	fn func() int
	c chan int
}

func workFn() int {
	fmt.Println("send a rq\n")
	return 0
}

func fp(rt int){
	fmt.Printf("receive a rt: %d\n", rt)
}

func requester(work chan <-Request) {
	c := make(chan int)
	for {
		time.Sleep(rand.Int63n(9*time.Second))
		work <- Request{workFn, c}
		result := <- c
		fp(result)
	}
}

type Worker struct {
	requests chan Request // buffered channel
	pending int // count of pending tasks
	index int   // index in heap
}

func (w *Worker) work( done chan * Worker ) {
	for {
		req := <- w. requests
		req.c <- req.fn()
		done <- w
	}
}

type Pool []*Worker

type Balancer struct {
	pool Pool
	done chan *Worker
}

func (b *Balancer) balance(work chan Request) {
	for {
		select {
		case req := <- work:
			b.dispatch(req)
		case w := <- b.done:
			b.complete(w)
		}
	}
}

func (p Pool) Less(i, j int) {
	return p[i].pending < p[j].pending
}

func (b *Balancer) dispatch(req Request) {
	w := heap.Pop(&b.pool).(*Worker)
	w.requests <- req
	w.pending++
	heap.Push(&b.pool, w)
}

func (b *Balancer) complete(w *Worker) {
	w.pending--
	heap.Remove(&b.pool, w.index)
	heap.Push(&b.pool, w)
}

func main() {
	fmt.Println("hello\n")
}
