package point2d

import "math"

type Precision struct {
	DIGITS  int = 3
	EPSILON float64 = math.Nextafter(1, 2) - 1
}

// Affine transformation in two dimensions
type AffineTransform struct {
	M [2][2]float64
	D [2]float64
}

// defines a two dimensional point object
type Point2D struct {
	x float64
	y float64
	m int64
	prec *Precison
}

func New(ax, ay float64; m int64; prec *Precision) *Point2D {
	if (prec = NIL)	
}  

// calculates the euclidic distance in two dimensions
func (a *Point2D) Distance(b Point2D) float64 {
	dx := (a.X - b.X)
	dx *= dx
	dy := (a.Y - b.Y)
	dy *= dy
	return math.Sqrt(dx + dy)
}

// calculate the euclidic distance in three dimensions
func (a *Point3D) Distance3D(b Point3D) float64 {
	dx := (a.X - b.X)
	dx *= dx
	dy := (a.Y - b.Y)
	dy *= dy
	dz := (a.Z - b.Z)
	dz *= dz
	return math.Sqrt(dx + dy + dz)
}


