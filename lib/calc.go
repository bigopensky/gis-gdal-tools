package calc

type LengthUnit float64

const (
	KM LengthUnit = 1E3 
	M  LengthUnit = 1E0
	DM LengthUnit = 1E-1
	CM LengthUnit = 1E-2
	MM LengthUnit = 1E-3
)

type Precision struct {
	digits  int = 3
	unit    LengthUnit = MM
	epsilon float64 = math.Nextafter(1, 2) - 1
}


