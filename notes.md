MARKDOWN NOTES (for markdown editor at https://dillinger.io/):

## Math

Inline math: $E = mc^2$

Block equations:

$$
\sum_{i=1}^{n} i = \frac{n(n+1)}{2}
$$

(Markdown makes it easy to format text. You can write in **bold**, *italic*, or ~~strikethrough~~. Combine them for ***bold italic*** text. Use `inline code` for technical terms.)
Blockquotes can contain other markdown elements:
> **Tip:** Use `Cmd+Shift+Z` to enter zen mode for distraction-free writing.


---





a/sineA = b/sineB = c/sineC

( a=opp, c=hyp, b=adj )

tangent θ = sine θ/cosine θ

# TRIG IMPLEMENTATION DESIGN NOTES.

tan θ = sin θ/cos θ

For a 90-degree rotation, you can swap the x and y components of the vector and negate one of them (either the new x or the new y, depending on the direction of rotation).

( 2,3 ) -> ( 3,-2 )

adj=2,opp=3,angle=56.31

tan(56.31)=3/2

## if you have an angle and the hypoteneuse, you can get the ratio between the opp and adj (thus the ratio between the sine and cos) from the tangent of the angle.

sine(angle) = opp/hyp
cos(angle) = adj/hyp
tan(angle) = opp/adj  (sine/cos)



sine (sin) = opposite/hypotenuse, cosine (cos) = adjacent/hypotenuse, and tangent (tan) = opposite/adjacent
csc = hyp/opp, sec = hyp/adj, and cot = adj/opp. Hyp is the longest side, which is across from the right angle, opp is the leg not touching the angle, and adj is the leg touching the angle.


If you have 2 angles and a side (say, 1deg and 90deg, and a hyp==1 ), you can calculate the other 2 sides, and in a rh triangle that includes the adj and opp.

angle_1 = 2
angle_2 = 90

angle_3 = 180 - 90 - 2 = 88

hyp = 1

hyp(2) = opp(2) + adj(2)

0.5(2) = 0.25
1 - 0.25 = 0.75, root(3/4) = 



opp(2)=1/4, adj(2)=9/16,


the Law of Sines tells you:
a/sinA =b/sinB =c/sinC = m
where m is that scale factor ... and also the diameter of the triangle's circumcircle.
 
 so if one of the angles is 90 (thus the sine is 1.0), 
 
 we could say that a/sinA == b/ sinB == c 

, or in other words a * c = sin A, b * c = sin B, if sin C == 1, which requires a rightangle triangle.

This makes side c the hypoteneuse. If the hypoteneuse is len(1), a == sine(A), b == sine(B), 

cos(angle) = adj/hyp. 


Sine (sin): sin(angle) = Opposite / Hypotenuse.

if sine(angle) == P, Opposite = Hypoteneuse * sine(angle). 

Cosine (cos): cos(angle) = Adjacent / Hypotenuse.
if cos( angle) == Q, Adjacent = Hypoteneuse * Q.

for fixed steps of (angle), and knowing the hypoteneuse (typically hyp = r = 1 ), Adjacent == Q == cos( angle ).


If we said a triangle pair had hypoteneuse' = 1, and arbitrary opp' of some precisionvalue, such as 0.0000001, we would know their origin angle' would have sine 0.0000001/1.

That is the ycoord of the hypoteneuse_opp corner on  a unit circle (where hyp=r=1 ).

To get the ycoord to 1, is to calculate sine(origin_angle=90)


a/sineA = b/sineB = c/sineC

(a=opp, c=hyp, b=adj )

As long as we have the hypoteneuse, and arbitrary opp, we have the adjacent, and the sine(origin_angle).

opp/sine(origin_angle) = hyp/1
adj/sine(B) = hyp/1.

a/sineA = hyp

hyp * a = sineA

sineA / a = hyp

if a == sineA, hyp = 1

Tangent (tan): tan(angle) = Opposite / Adjacent. 



rightangle triangle:

if we have the origin (hyp&adj) angle,
and the hypoteneuse, if we could get the adj and opp, the opp length would give us the ycoord that is the sine point on the curve for that angle.

The best way to do this is to choose a circle where the radius (hypoteneuse) allows us to determine the adjacent for a plugged-in angle, thus the opp.

if c2 = a2 + b2, and we have c2 and O (angle), we might be able to convergently estimate an adjacent, thus opp.




// Half-Angle Formulaes.

The half-angle formulas in trigonometry relate the trigonometric functions of an angle to those of half that angle. They are useful for finding exact values of trigonometric functions for angles that are not standard multiples (like 30°, 45°, 60°), or for simplifying expressions. 
Key Formulas:
Sine: sin(x/2) = ±√[(1 - cos(x))/2] 
Cosine: cos(x/2) = ±√[(1 + cos(x))/2] 
