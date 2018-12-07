# Investigation
``` Bash
echo a a > a.txt
echo a a > b.txt
# cat a.txt == "a a"
# cat b.txt == "a a"

# This means that the most recent output descriptor gets the output
echo a > a.txt > b.txt
# cat a.txt == ""
# cat b.txt == "a"
```

```
# This means chains can accept input
(echo "a" && echo "b" && echo "a") | (sort | sort) | sort

# Result:
# a
# a
# b
```
