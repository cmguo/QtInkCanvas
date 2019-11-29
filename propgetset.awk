NR==1 { 
    gsub("\\r", "")
    gsub("\\n", "")
    type=$1
    name=$2
    print $0 "()"
}

/^ {8}get \{/ {
    gsub("get \\{ ?", "")
    gsub(" ?\\}", "")
    print $0
}

/^ {8}get/ {
}

/^ {8}set \{/ {
    print "    }"
    print "    void Set" name "(" type " value)"
    print "    {"
    gsub("set \\{ ?", "")
    gsub(" ?\\}", "")
    print $0
}

/^ {8}set/ {
    print "    }"
    print "    void Set" name "(" type " value)"
    print "    {"
}

/^ {4}\{/ {
    print $0
}

/^ {4}\}/ {
    print $0
}

/^ {12}/ {
    print substr($0, 4)
}

END {
    print "\n"
}
