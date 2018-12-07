# Investigation
``` Bash
echo a a > a.txt
echo a a > b.txt
# cat a.txt == "a a"
# cat b.txt == "a a"

echo a > a.txt > b.txt
# cat a.txt == ""
# cat b.txt == "a"
```

```
(echo "a" && echo "b" && echo "a") | (sort | sort) | sort
# Result:
# a
# a
# b
```
