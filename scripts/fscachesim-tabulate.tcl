#!/usr/bin/tclsh

proc arrayPuts name {
    upvar $name a
    foreach element [lsort [array names a]] {
	puts "$element = $a($element)";
    }
}

proc typeGet {stanza} {
    set name [lindex $stanza 0]

    regexp -nocase {(^[a-z0-9]+)[\.]?} $name match type

    return $type
}

proc nameGet {stanza} {
    set name [lindex $stanza 0]

    regexp -nocase {^[a-z0-9]+[\.](.*)} $name match subName

    return $subName
}

proc basename {name} {
    set path [split $name "/"]

    return [lindex $path [expr [llength $path] - 1]]
}

proc meanGeo {values} {
    set mean 1

    foreach i $values {
	set mean [expr $mean * $i]
    }

    return [expr pow($mean,[expr 1 / double([llength $values])])]
}

proc perOrigParse {stanza results} {
    upvar $results perOrig

    for {set i 1} {$i < [llength $stanza]} {incr i 1} {
	regexp {(.*)\=(.*)} [lindex $stanza $i] match name val
	set perOrig($name) $val
    }
}

proc StoreCacheParse {stanza readHits readMisses demoteMisses} {
    upvar $readHits rh
    upvar $readMisses rm
    upvar $demoteMisses dm

    perOrigParse [lindex $stanza 2] rh
    perOrigParse [lindex $stanza 3] rm
    perOrigParse [lindex $stanza 6] dm
}

proc StoreCacheSegParse {stanza readHits readMisses demoteMisses} {
    upvar $readHits rh
    upvar $readMisses rm
    upvar $demoteMisses dm

    StoreCacheParse [lindex $stanza 5] rh rm dm
}

proc StoreCacheSimpleParse {stanza readHits readMisses demoteMisses} {
    upvar $readHits rh
    upvar $readMisses rm
    upvar $demoteMisses dm

    StoreCacheParse [lindex $stanza 4] rh rm dm
}

proc resultsParse {resultsRaw resultsParsed} {
    upvar $resultsParsed stats

    set resultsArray [lindex $resultsRaw 0]

    if {[string compare "StoreCacheSeg" [typeGet $resultsArray]] == 0} {
	StoreCacheSegParse $resultsArray \
	    readHitsArray readMissesArray demoteMissesArray
    } elseif {[string compare "StoreCacheSimple" [typeGet $resultsArray]] == 0} {
	StoreCacheSimpleParse $resultsArray \
	    readHitsArray readMissesArray demoteMissesArray
    } else {
	error "Unrecognized StoreCache subclass"
    }

    set nameClients {};
    for {set i 1} {$i < [llength $resultsRaw]} {incr i 1} {
	set name [nameGet [lindex $resultsRaw $i]]

	lappend nameClients $name
	StoreCacheSimpleParse [lindex $resultsRaw $i] \
	    readHitsClients readMissesClients demoteMissesClients
    }

    foreach i $nameClients {
	set rhc 0
	catch {
	    set rhc $readHitsClients($i)
	}
	set rmc $readMissesClients($i)

	set rha 0
	catch {
	    set rha $readHitsArray($i)
	}

	set rma $readMissesArray($i)

	set dma 0
	catch {
	    set dma $demoteMissesArray($i)
	}

	set total [expr $rhc + $rmc]

	set rhrc [expr double($rhc) / $total]
	set rhra [expr double($rha) / $total]
	set dmra [expr double($dma) / $total]

	set lat [expr 0.2 * ($rhra + $dmra) + 5.2 * (1 - $rhrc - $rhra)]
	set latzero [expr 0.2 * $rhra + 5.2 * (1 - $rhrc - $rhra)]

	set rhrc [format "%.2f" $rhrc]
	set rhra [format "%.2f" $rhra]
	set dmra [format "%.2f" $dmra]

	set stats($i) \
	    [list \
		 $rhc \
		 $rmc \
		 $rha \
		 $rma \
		 $dma \
		 $total \
		 $rhrc \
		 $rhra \
		 $dmra \
		 $lat \
		 $latzero]
    }
}

if {$argc != 2} {
    error "Usage: fscachesim-tabulate.tcl baselineFilename expFilename"
}

set baseFilename [lindex $argv 0]
set expFilename [lindex $argv 1]

set baseFileH [open $baseFilename r]

set baseText {};

while {[gets $baseFileH line] >= 0} {
    lappend baseText $line
}

close $baseFileH

eval set baseList {[join $baseText " "]}

resultsParse $baseList baseStats

set expFileH [open $expFilename r]

set expText {};

while {[gets $expFileH line] >= 0} {
    lappend expText $line
}

close $expFileH

eval set expList {[join $expText " "]}

resultsParse $expList expStats

foreach client [lsort [array names baseStats]] {
    lappend baseLat \
	[format "%.2f" [lindex $baseStats($client) 9]]
    lappend expLat \
	[format "%.2f" [lindex $expStats($client) 9]]
    lappend expSpeedup \
	[format "%.2f" [expr [lindex $baseStats($client) 9] / [lindex $expStats($client) 9]]]
#     lappend expLatZero \
#  	[format "%.2f" [lindex $expStats($client) 10]]
#     lappend expSpeedupZero \
# 	[format "%.2f" [expr [lindex $baseStats($client) 9] / [lindex $expStats($client) 10]]]
}

lappend expSpeedup [format "%.2f" [meanGeo $expSpeedup]]

# lappend expSpeedupZero [format "%.2f" [meanGeo $expSpeedupZero]]

puts [basename $baseFilename]

puts -nonewline "Mean lat\t"
puts [join $baseLat "\t"]

puts [basename $expFilename]

puts -nonewline "Mean lat.\t"
puts [join $expLat "\t"]

puts -nonewline "Speedup\t\t"
puts [join $expSpeedup "\t"]

# puts [join $expLatZero "\t"]
# puts [join $expSpeedupZero "\t"]
