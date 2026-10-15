# APLIB
A lightweight arbitrary-precision arithmetic library, partially-complete. The operators, the very machine, is being moulded.

A mix of native (IEEE754, int, etc), and ap (base2, base10, converters) functions and methods.
Supports Trigonometry functions, such as sine(a), cosine(a), tangent(a), etc.
I am designing APLIB to be as portable across target systems, as fast, and as lightweight, and as useful, as possible.

## Is APLIB right for you?

Do you...

-[x] ...want correct, arbitrary-precision arithmetic results?
-[x] ...want the arithmetic operators to be fast, even with many, many digits?
-[x] ...want easy interoperability with native types such as int, float and double?
-[x] ...want Trig functions that are more accurate, and calculate faster than, the stuffy stdlib math module that ships with your compiler?
-[ ] ...appreciate skill in other practitioners, and you feel impressed?
-[ ] ...like my style, the 'cut of my jiff', and want to make me happy?
-[ ] ..have waited your entire life to find "the chosen one", that one programmer you can truly admire and understand is far superior to you, without there being any jealousy involved?

## Choose APLIB. Choose Life.
Below is you, at maths, when you use my APLIB library:

![alan_image](https://i.ytimg.com/vi/5KBN8xmkhxo/maxresdefault.jpg)


## Quotes.

> "I used to think it was about the music. Is it ecker's like.
> It's about the people!!"
> ( "We'll be singing, we'll be winning..." )
>
> Pete Postlethwaite.


> “The greatest wealth is to live content with little.”
>
> — Plato.


## Usage Quick Guide
```c
InitAPLIB();

// NB: The AP type is itself a pointer (T*) type.
// AP* is indeed a pointer-to-a-pointer type.
// It's a helpful facility so you don't have to construct the result
// object (itself a pointer) yourself.
// The RVALUE operands (LHS, RHS), do need to be generated.

// Usage example:
AP LHS = aplib->SimpleAP( "+12.32783" );
AP RHS = aplib->SimpleAP( "+2345.321" );
AP* Result;
aplib->add( LHS, RHS, Result, info );
/* The result of '12.32783 + 2345.321' is now in *Result, so: */
AP V = *Result;
```

## Sample Functions

| Function | Operation |
|----------|--------|
| aplib->root( V, B, R, flags ) | Newton's Method for approximating roots. |
| aplib->reciprocal( V, B, R, flags ) | V -> (1/V) -> R |
| aplib->sine( V, R, flags ) | V: angle in radians, R: sine(V) in radians   |

## Finding The Source Code
[Github]: https://github.com/
The Github repo can be found at: [github.com/inventordave2](https://github.com/inventordave2/RB)
Search for something even better, not that it exists: [Github]

## Addendum

Reading about Trigonometry [^1] grows the mind. Reading about people [^2] can also grow the mind.

[^1]: https://www.mathsisfun.com/algebra/trigonometry.html
[^2]: https://www.waterstones.com/book/kindred/rebecca-wragg-sykes/9781472937476 

*This library is part of a solo dev project by myself, "Inventor Dave", to build a set of modules that can replace a User's existing choice of compiler vendor and toolset.*
