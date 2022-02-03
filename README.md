# cubecell htcc-ab02s GPS serial test

## Wrong GPS data recieve

Serial port 1 loses GPS data packets if you send data through port 0 at the same time as receiving GPS data.
Programm printing "0" to Serial0 and at the same time checking recieved nmea checksum data. If it ok is printing "1". Else printing "error".

Programm protocol:

```
Write something to Serial0.
0000000000000000000000000000000000000000000000000
0000000000000000000000000000000
ERROR 49<>0 $GNGGA,,01
0000000000000000000000000000000000000000000000000000000000000000000000

Silent mode to Serial0.
111111111111111111111111111111

Write something to Serial0.
00000000000000000000000000000000000000000000000000
ERROR 2A<>7A $GNGGA,,,N\*7A
```

If you do something longer than 5 ms - rx buffer overloading and some data lost.
