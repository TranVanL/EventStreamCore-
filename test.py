#!/usr/bin/env python3
import asyncio
import os
import time
import argparse
import multiprocessing as mp

try:
    import uvloop
    asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())
except ImportError:
    print("uvloop not installed, run: pip install uvloop")

async def sender(host, port, payload, rate, counter):
    try:
        reader, writer = await asyncio.open_connection(host, port)
        interval = (1.0 / rate) if rate > 0 else 0.0
        next_send = time.perf_counter()

        while True:
            writer.write(payload)
            # tăng counter toàn cục
            with counter.get_lock():
                counter.value += 1
                total = counter.value

            # in ra tổng số message đã gửi từ đầu chương trình
            print(f"Total messages sent: {total}")

            if interval > 0:
                next_send += interval
                delay = next_send - time.perf_counter()
                if delay > 0:
                    await asyncio.sleep(delay)
            else:
                await asyncio.sleep(0)
    except Exception as e:
        print("Error:", e)
        await asyncio.sleep(0.1)

def worker(host, port, payload, rate, counter):
    async def run():
        await sender(host, port, payload, rate, counter)
    asyncio.run(run())

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", required=True)
    parser.add_argument("--port", required=True, type=int)
    parser.add_argument("--procs", type=int, default=4, help="Số process song song")
    parser.add_argument("--size", type=int, default=1024)
    parser.add_argument("--rate", type=float, default=0.0, help="msg/s mỗi process (0=max)")
    args = parser.parse_args()

    payload = os.urandom(args.size)

    # counter toàn cục cho tất cả process
    counter = mp.Value('i', 0)

    procs = []
    for _ in range(args.procs):
        p = mp.Process(target=worker, args=(args.host, args.port, payload, args.rate, counter))
        p.start()
        procs.append(p)

    try:
        for p in procs:
            p.join()
    except KeyboardInterrupt:
        for p in procs:
            p.terminate()

if __name__ == "__main__":
    main()
