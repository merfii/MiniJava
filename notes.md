major_version不能大于49 否则会需要stackMap检查，变得复杂

词法分析器的返回值为枚举数字。能否用这些数字索引指向地址数组进行查表，得到指针跳转执行。以消除判断进行优化。
修改关键字switch及语法结构

switch(index_value,min,max)
{
	case 1: codeblock1...
	case 2: codeblock2...
	....
	default:codeblockDefault...
}
由于指定了index_value的最大值和最小值，因而可以自动生成给定的索引数组。
问题是，通过这样消除多级判断，却仍然存在一个间接寻址指令和寄存器跳转指令，真的能加快效率吗？

关键字重复太多，不敢使用enum直接引用。最后选择封装到一个Symbol类里.这是新的C++11特性enum class的作用的最好体现。

问题：关键字的平均长度是否对编译器性能有重大影响？如是，则可以通过建立多叉树减少字符串匹配尝试次数，优化关键字匹配效率.

由于ByteCode代码的树状结构，故利用C++的STL库和类继承机制，完全按照定义对所有数据进行存储。最后进行树的递归遍历，一并生成代码。
需要特殊注意管理常量池,序号不能变动只能增加，重复元素重复利用;每个结构都有length字段，必须小心地向上传递生成。

对于C++ char类型的数字  转为16进制形式输出，为什么有时会出现0xffffffca这种情况;在大多数情况下是正常的,输出代码如下：
textFile<<setw(4)<<"0x"<<hex<<(unsigned int )ch[0]

在对于构造树形结构的顺序存在矛盾的要求。
一方面，内存管理要求必须以“最终持有对象者”作为对象创建者，由父亲结点负责创建并管理子结点。
使得整个树以从上至下的顺序构造并分配内存。否则，子结点传入父亲结点时，值引用导致构造-析构开销;参引用导致悬指针或内存泄漏。
另一方面，逻辑结构上，父亲结点会使用子结点的index。因此必须提够子结点的信息，父亲结点的某系成员变量才能填入。
例如，在method对象内创建CodeAttribute对象数组,在CodeAttribute对象内创建Instruction数组。而method对象里又有uint16 nameindex字段，
需要首先将方法名填入常量池，从常量池获得其索引下标，才能填写nameIndex字段。
子结点需要根结点的句柄用来新增常量池、输出Byte码。用于要重载赋值运算符=，不能出现引用成员变量！必须用指针
向容器中添加对象时避免复制的唯一方法是使用指针？

为了管理常量池 采用map进行字符串的重复判定 采用list进行标号到字符串的索引

符号表与常量池有何区别？能否合并？ 不能！因为：
区别：
常量池是用于.class文件生成的所有字符串
符号表是编译时的扫描索引
常量池负责从编号到字符串
符号表负责从字符串映射到类型、值;跨越类对类名进行索引;
常量池负责对于成员变量类型、成员函数的描述
符号表负责对于局部变量作用域的管理，常量池不含局部变量


编码问题
.class内部使用Utf8
Linux下普遍使用Utf8
windows环境下，主要使用ANSI(中文GB2312)
不论哪种编码，在只涉及英文时，与ASCII码相同,故可直接用于.class文件
但是不能用Utf16，因为Utf16早期版本为两字节定长，后期为至少两字节变长

Java对外采用Utf16(windows称unicode)故在编写miniJava compiler时需要对字符集问题做特殊处理


由于符号表内容的结构类型不一样，故需要两次查询
第一次从字符串->(类型，索引)
第二次根据类型从对应的数组取得相应符号的其他信息

对于多层对象传参，可以用f(const string& str)避免反复构造。const的目的是使得f(string("合法"))

为了处理多层花括号对应变量不同:
第1层全是类名;第二层是成员变量和成员函数;第3层才是函数内部，设立bracketPush() bracketPop()函数对于符号表内的分层栈进行管理

JVM函数调用规范：
父frame:  stack: ..., objectref, [arg1, [arg2 ...]] => ...
子frame:  stack:空  静态变量 0-当前对象句柄 1-arg1  2-arg2 ...
返回值
子frame: stack ...,返回值
父frame: stack .... => ...,返回值

跳转指令是相对寻址
写入地址是相对跳转指令的int16_t强制转换为uint16_t

miniJava文法有规则
ArrayLookup::=PrimaryExpression "[" PrimaryExpression "]"
不如改成
ArrayLookup::=Identifier"[" PrimaryExpression "]"

( VarDeclaration )* ( Statement )*
在VarDeclaration=Identifier处出现二义性，故整体处理

MessageSend::=PrimaryExpression"." Identifier "("( ExpressionList )? ")"
函数调用有三种情况 this.func()  对象.func()  (new ClassName()).func()
需要进行成员函数的存在性和类型检查，故PrimaryExpression需要进行传递综合属性
又由于BracketExpression::="("Expression")"规则的存在，故原则上需要对于Expression、PrimaryExpression全部符号定义"类名"这一综合属性并逐级向上传递
事实上只有个别的规则推导使用到了该属性，这样实现对于非动态语言很麻烦，也会造成函数返回值大量冗余
这里选择打破语法的推导规则，简单地从语法分析器类中置成员变量instanceRef，储存对象的属性信息(如何置为失效？)
由于第一条执行的语句总是ptintfln((new ClassName).XXXX())括号内跟的类尚未声明，故对于new 前端不再进行类的检查。交给jvm了


相同的文法出现在不同的位置，语义是不同的，如
"class" Identifier "{" ( VarDeclaration )*( MethodDeclaration )* "}"
MethodDeclaration ::= "public" Type Identifier 
"("( FormalParameterList )? ")" "{"( VarDeclaration )* ( Statement )*"return" Expression ";" "}"
其中都包含VarDeclaration,但一个是成员变量，一个是局部变量，二者处理起来是不一样的
又如，上述MethodDeclaration中出现了Type作为返回值声明，又出现在FormalParameterList中作为参数类型声明，二者的作用也是不一样的，需要不同的处理


由于需要跳过错误继续执行，也就是需要扫描到最近的终止符号，比如;})]，尤其注意，文件没有出现这些东西，则语法分析器将一直收到EOF死循环
所以，所有扫面的地方，必须添加EOF终止条件
这里，我通过在词法分析器中，添加返回EOF两次的判断，则抛出异常EofError 在主函数中捕获 避免了各种程序的死循环

throw exception的主要作用不在于报错，而在于改变程序的执行流程，跨越多层函数返回，而不需要经过中间函数。

继承
由于有继承，所以会出现使用子类对象调用父类的方法的情况，动态绑定虚拟机完成，但前端必须说明函数的类型参数、变量类型
这只能由链状向上扫描继承树得到。在类生成器中实现一个符号表的链状结构。


以下语法在java中正确，在miniJava中都是不正确的

public static void main(String[] args) {
	int a;
	System.out.println(a);
}

a+b*c

a>b

while(a>8&&b>10)

public void func()

public int func()
{
	if(a)
		return 1;
	else
		return 2;
}

无public


在C++中打开（创建）输出的.class文件时，应调用函数
open(fileName,std::ios::out|std::ios::trunc|std::ios::binary);
其中trunc是覆盖标志  binary是以二进制方式打开
若忘记使用binary标志，则在linux、max、windows下调用put(uint8_t ch)函数将有不同的行为
其中linux mac将会如实输出单字节 windows当ch=0x0A时输出0x0D 0x0A两个字节
估计是由于自动进行编码转换导致。

----------------------
内码是程序内部使用的字符编码，特别是某种语言实现其char或String类型在内存里用的内部编码；
外码是程序与外部交互时外部使用的字符编码。“外部”相对“内部”而言；不是char或String在内存里用的内部编码的地方都可以认为是“外部”。例如，外部可以是序列化之后的char或String，或者外部的文件、命令行参数之类的。

Java语言规范规定，Java的char类型是UTF-16的code unit，也就是一定是16位（2字节）；

    char, whose values are 16-bit unsigned integers representing UTF-16 code units (§3.1).

然后字符串是UTF-16 code unit的序列：

    The Java programming language represents text in sequences of 16-bit code units, using the UTF-16 encoding.

这样，Java规定了字符的内码要用UTF-16编码。或者至少要让用户无法感知到String内部采用了非UTF-16的编码。
“感知”可以是多方面的，例如随机访问某个下标的code unit（String.charAt()）应该是O(1)操作，这只有使用UTF-16或者别的“定长”编码才可以做到。注意我这里说的“定长”特指code unit定长，而不是说code point定长。

String.getBytes()是一个用于将String的内码转换为指定的外码的方法。无参数版使用平台的默认编码作为外码，有参数版使用参数指定的编码作为外码；将String的内容用外码编码好，结果放在一个新byte[]返回。

题主的例子里，显然外码是UTF-8，那么调用了String.getBytes()之后得到的byte[]只能表明该外码的性质，而无法碰触到String内码的任何特质。

另举一例：
Java标准库实现的对char与String的序列化规定使用UTF-8作为外码。Java的Class文件中的字符串常量与符号名字也都规定用UTF-8编码。这大概是当时设计者为了平衡运行时的时间效率（采用定长编码的UTF-16）与外部存储的空间效率（采用变长的UTF-8编码）而做的取舍。 



举个例子。“ABC”这三个字符用各种方式编码后的结果如下：
UTF-16BE 				00 41 00 42 00 43
UTF-16LE 				41 00 42 00 43 00
UTF-16(Big Endian) 		FE FF 00 41 00 42 00 43
UTF-16(Little Endian) 	FF FE 41 00 42 00 43 00

UTF-8用1～4个字节来表示代码点。表示方式如下：
UCS-2 (UCS-4) 				位序列			第一字节	 	第二字节	 	第三字节	 	第四字节
U+0000 ..U+007F 	00000000-0xxxxxxx 		0xxxxxxx 			
U+0080 ..U+07FF 	00000xxx-xxyyyyyy 		110xxxxx 	10yyyyyy 		
U+0800 ..U+FFFF 	xxxxyyyy-yyzzzzzz 		1110xxxx 	10yyyyyy 	10zzzzzz 	
U+10000..U+1FFFFF 	00000000-000wwwxx-
					xxxxyyyy-yyzzzzzzz 		11110www 	10xxxxxx 	10yyyyyy 	10zzzzzz
					

	