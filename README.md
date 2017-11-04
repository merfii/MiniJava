**Mini Java** is my final assignment of course <Compiler Technique>, written in C++, compiled by MSVC 2010.

[Mini Java](http://www.cambridge.org/resources/052182060X/) is a subset of Java. The meaning of a MiniJava program is given by its meaning as a Java program. Overloading is not allowed in MiniJava. The MiniJava statement System.out.println( ... ); can only print integers. The MiniJava expression e.length only applies to expressions of type int []. 


Several clippings were made comparing to Java.
- No overloading supporting.
- Only one method of main class in a single file, and must titled "public static void main(String[])".
- Only one output "System.out.println( int )" supported.
- No interface.
- Only variables and methods be decleared in class.
- Variables only hava 4 types. int, bool, int array, object. All variables are auto.
- Only public methods supported. Method must return a value.
- 6 kinds of statement: block, assignment statement, array assignment statement, print statement, if, which.
- 9 kinds of expression: and, compare, plus, minus, times, array lookup, array length, message sending, primary expression.
- No constructor. Initiallize by empety parameter.
- No comments.
- No variable initialization during declearation.
- No internal class.

For precise definition of Mini Java, refer to [BNF](http://www.cambridge.org/resources/052182060X/MCIIJ2e/grammar.htm)
