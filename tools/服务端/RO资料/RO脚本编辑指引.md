# RO 脚本编辑指引

> RO 所有脚本都定义为【NPC】，脚本语法无限接近 C 语言，英文原文参见 [doc/script_commands.txt](../../../doc/script_commands.txt)

------

## 基础知识

### 如何添加自己制作的 NPC

做一个脚本放到 [npc](../../../npc) 目录里面如：`商人.txt`

~~然后在 [conf/map_athena.conf](../../../conf/map_athena.conf) 里面添加相对路径和文件名，举例：~~

然后在 [npc/re/scripts_main.conf](../../../npc/re/scripts_main.conf) 找到对应 npc 分类下的 `*.conf`，再在该 `*.conf` 文件里面添加相对路径和文件名，举例：

```
// 添加自定义的 NPC 脚本（不需要运行的脚本在 npc 前输入 // 注释表示关闭）
npc: npc\商人.txt
npc: npc\npc1\商人1.txt
npc: npc\npc1\npc2\商人2.txt
```

### 如何设定 NPC

NPC 脚本基本格式：

`<地图名称>,<横坐标>,<纵坐标>,<面对方向>	<类型>	<名称>	<造型>,`

其中：

- 面对方向、类型、名称、造型 之间为制表符`	`（`\t`）
- 地图名称： 可以在 [RO321](http://ro.ro321.com/index.php?page=areainfo&area=5999) 查询，也可以进入游戏输入命令 `/where` 可查当前地图英文名，
- 类型：只有两个可选值，`shop` 表示商店，`script` 表示脚本。
- 造型：造型编号可参考《[AzureFlame](http://www.usamimi.info/~blaze/npc/npc_all.html)》、或《[Eathena.net脚本指令大全.chm](./Eathena.net脚本指令大全.chm)》中【其他资料】的【NPC 代码一览表】，但注意不要选客户端没有的造型，否则会报错。另外，若造型编号为 `-1` 表示为事件，可以被其他命令调用；类型为 `shop` 的造型不可为 `-1`。
- 面对方向：以小地图方向为准，假设 NPC 为 X，其面对方向编号为：

```
2 1 8
3 X 7
4 5 6
```


### 关于 NPC 脚本类型的示例

1. `shop` 商店脚本：此 NPC 被点击后出现一个卖红、黄药水的商店：

```
prontera,167,204,6 shop 特惠店     96,501:20,502:30
```


2. `script` 常规脚本：此 NPC 被点击后出现一个内容为 “脚本例子” 的窗口：

```
prontera,167,204,6 script 测试脚本     96,{
    mes "脚本例子";
    close;
}
```


3. `script` 触发式脚本：在玩家进入其坐标上下左右各 10 （最大值为 20）的范围内将出现一个内容为 “脚本例子” 的窗口：

```
prontera,167,204,6 script 测试脚本     96,10,10,{
    mes "脚本例子";
    close;
}
```


### NPC 脚本编制基础

脚本在定义完 NPC 参数后，其执行逻辑需要写在 `{}` 内，具体参考上面的脚本范例。

- 脚本程序主要由语句和标签组成。
- 语句用于执行各种命令，每执行完一行顺序执行下一行，每一行语句以 `;` 结束。
- 标签用于脚本内某段开始执行，可以使用菜单或者条件判断语句进行跳转到标签。
- 标签名称后用 `:` 作为标记（注意，标签名称区分大小写）。
- 脚本程序中可对各种变量进行判断和操作，变量有服务端程序内置的系统变量以及规范化的自定义变量


### 系统变量说明

系统变量可参考 db 文件夹中的 [db/const.txt](../../../db/const.txt)。

常用的系统变量如下（使用自定义变量的时候必须注意不得与其冲突，因为系统变量具有绝对优先权）：

- `StatusPoint`：属性点数
- `BaseLevel`：基本等级     
- `SkillPoint`：技能点数
- `Class`：职业
- `Zeny`：金钱
- `Sex`：性别
- `Weight`：当前负重
- `MaxWeight`：最大负重
- `JobLevel`：职业等级
- `BaseExp`：基本经验值
- `JobExp`：职业经验值
- `NextBaseExp`：基本等级升至下一级所需经验值
- `NextJobExp`：职业等级升至下一级所需经验值
- `Hp`：当前生命值HP
- `MaxHp`：最大生命值HP
- `Sp`：当前法力值SP
- `MaxSp`：最大法力值SP

### 自定义变量说明

- 变量名称可使用任意大小写字母、数字以及下划线等的组合，但是必须以字母开头。
- 合法名称举例：`Abc`，`D12`，`f_d3`
- 非法名称举例：`G+D3`，`F#e`，`3Di`，`_5e`，`ａ３`，`变量1`
- 为了扩展变量的用途，可以在变量加上前后缀
- 可用的前缀有 `@`、`#`、`$` 三种
- 可用的后缀有 `$`
- 没有任何前缀，表示变量与玩家单个角色相关，并保存在数据库中。
- 前缀 `@` 表示变量为临时使用，不保存在数据库中。
- 前缀 `#` 表示变量与玩家账号相关，玩家同一账号下所有角色都可以使用（变量保存在数据表 `ragnarok/acc_reg_num`）。
- 前缀 `$` 表示变量为服务器变量，对所有玩家都有效（变量保存在数据表 `ragnarok/mapreg`）。
- 除了 `#` 和 `$` 不能重叠使用，其他可以重叠，功能为两者的叠加，但顺序要求 `@` 在前。
- 后缀 `$` 表示变量为字符串，其值不限于数值，但是数字也当作字符串处理。


## 常用的属性变更指令

### jobchange 职业变更

职业代码表：

| 职业代码 | 职业 |
|:---:|:---:|
| 0 | 初学者 |
| 1 | 剑士 |
| 2 | 法師 |
| 3 | 弓箭手 |
| 4 | 服士 |
| 5 | 商人 |
| 6 | 盗賊 |
| 7 | 骑士 |
| 8 | 牧师 |
| 9 | 魔法师 |
| 10 | 铁匠 |
| 11 | 猎人 |
| 12 | 刺客 |
| 13 | 骑士 |
| 14 | 十字军 |
| 15 | 武僧 |
| 16 | 贤者 |
| 17 | 流氓 |
| 18 | 炼金术士 |
| 19 | 诗人 |
| 20 | 舞者 |
| 21 | 十字军 |
| 22 | 结婚造型 |
| 23 | 超级初心者 |

如命令 `jobchange 0;` 转职成初心者。

如命令 `jobchange 0,1;` 转职为进阶初心者，后面的 `1` 为附加职业代码：

- 0：普通职业
- 1：进阶职业
- 2：子嗣职业（被认养后的职业，尚未开放）


### setlook 设置形象

设置发型、发色、衣服颜色的命令。

具体可以看一下现有脚本。


### set 赋值命令

示例 `set @value,1;`： 将 `1` 赋值到 `@value` 。

可以给自定义变量或系统变量赋值，变量的定义方式参考前面【系统变量说明】和【自定义变量说明】。


### cutin 绘图命令

示例 `cutin "kafra_02",2;`： 显示指定的图片在右下角，其中数字代表位置代码：

- 0： 左下
- 1： 中下
- 2： 右下
- 3： 未知.........
- 4： 中间可移动和关闭
- 255： 清除

> 用于显示 NPC 的大图片，比如卡普拉那些漂亮 MM 。

### heal 回复命令

示例 `heal 100,100;`： 按数值回复 HP 和 MP，回复 100HP、100SP 


### percentheal 回复命令

示例 `percentheal 100,100;`： 按比例回复 HP 和 MP，回复 HP100%、SP100% 


### checkweight 重量检查

示例 `if (checkweigh(502,100) > 1) goto L_YES;`

- 两个参数分别为： 物品代码和数量
- 命令作用是检查拿了这些物品后是否超重，若超重则返回 `0`，否则返回 `1`


### readparam 读取参数

支持读取的参数（变量）在 [db/const.txt](../../../db/const.txt) 中说明。


### strcharinfo 人物信息读取

示例 `strcharinfo(0)`： 返回值为人物名称。

- 0： 角色名
- 1： 队伍名
- 2： 工会名


### getcharid() 人物 ID 读取

示例 `getcharid(0)`： 返回值为人物 ID。

- 0： 角色 charid
- 1： 队伍 ID
- 2： 工会 ID


### bonus 附加效果 

示例：

- `bonus bVit,3;`： 附加 VIT 3 点
- `bonus2 bAddRace,6,50;`： 附加不死属性抗性 +50%


### skill 学习技能


示例 `skill 140,1,0;`： 

- 参数依次为：技能代码,等级,(标识1)
- 表示学习第140号（技能代码参看 [`skill_db`](../../../db/re/skill_db.txt) ）技能，等级为1

### getskilllv 获取技能等级

示例 `getskilllv(142)`： 返回 142 号技能的等级


### basicskillcheck 

示例 `basicskillcheck`： 读取 [`battle_athena.conf`](../../../conf/battle_athena.conf) 中 `basic_skill_check` 项的值，并返回 


### setoption 设置状态

设置状态，参数请参考 GM 命令 `@option` 的 Z 项。

- 1： 火狩
- 2： 隱暱
- 4： 隱身
- 8： 手推車
- 16： 带老鹰
- 32： 骑鸟
- 64： 隱身

在实际使用中，需要注意的是，状态可以叠加的：

- 示例 `setoption 0;`： 恢复普通状态，即无任何特殊状态（无手推车无鹰无骑鸟）
- 示例 `setoption 3;`： 变成 “火狩 + 隐匿” 状态（3 = 1 + 2）


### sc_start

开启特殊状态，它有三个参数分别为：类型，参数1(技能等级)，参数2(暂时未用)

示例 `sc_start SC_FREEZE,1,0;`： 变成冰冻状态

> 支持的状态表见 [SC 技能状态一览](./SC状态一览表.md)


### sc_end

特殊状态关闭


### 重置所有属性点或技能点

- `resetstatus;`： 重置所有属性点 
- `resetskill;`： 重置所有技能点 


```
// 示例：重置辅助人员
prontera.gat,146,192,4 script 重置辅助人员 763,{
    mes "[重置辅助人员]";
    mes "我专门负责重置点数";
    mes "你想重置什么呢？";
    next;
    menu "^FF3355技能点(费用50w)^000000",L1,"^FF3355属性点(费用50w)^000000",L2,"^FF3355技能点和属性点(费用80w)^000000",L3,"取消",LEnd;
    L1:
        if (Zeny<500000) goto NeedZenys;
        mes "[重置辅助人员]";
        mes "已经重置好了";
        mes "^FF3355请好好分配^000000";
        set Zeny,Zeny-500000;
        resetskill;
        close;
    L2:
        if (Zeny<500000) goto NeedZenys;
        mes "[重置辅助人员]";
        mes "已经重置好了";
        mes "^FF3355请好好分配^000000";
        set Zeny,Zeny-500000;
        resetstatus;
        close;
    L3:
        if (Zeny<800000) goto NeedZenys;
        mes "[重置辅助人员]";
        mes "已经重置好了";
        mes "^FF3355请好好分配^000000";
        set Zeny,Zeny-800000;
        resetstatus;
        resetskill;
        close;
    NeedZenys:
        mes "[重置辅助人员]";
        mes "穷人不要来凑热闹！影响我做生意";
        close;
    LEnd:
        close; 
}
```


### changebase

改变人物显示的职业，但实际职业不变，可用于结婚系统 

示例 `changebase 22;`： 变成结婚人物形象


### 人物属性提升

- 示例 `statusup bStr;`： 参数提升 1 点
- 示例 `statusup bStr,10;`： 参数提升 10 点

常用可选参数 `bStr`，`bAgi`，`bVit`，`bInt`，`bDex`，`bLuk`，其他可查看 [db/const.txt](../../../db/const.txt)。



## 流程控制

### next 刷新指令 

示例 `next;`： 产生按钮 "下一步"，点击后重新输出


### close 关闭指令

示例 `close;`： 产生按钮 "关闭"，点击后关闭对话框


### menu 菜单指令

示例 `menu "好的",L_YES,"不好",L_NO;`： 

- 产生选择列表，有2个选项 "好的"、"不好"，选择后即发生跳转
- 选择 "好的" 跳转至 `L_YES` 这个标签，选择 "不好" 跳转至 `L_NO` 这个标签


### goto 跳转指令

示例 `goto L_YES;`： 直接跳转至 `L_YES` 标签。

平时使用时，常跟在 `if` 等条件判断语句后，也可以单独使用。

> `goto` 语句后的标签是区分大小写的，若出错则会使 `map-server` 进程挂起，且不会自动重启。


### if 条件判断指令

- 示例 `if(@value==1) goto L_YES;`： 如果 `@value=1`，跳转到 `L_YES`，否则继续执行下一行（用于判断的条件必须放置在`()`之中）。
- 示例 `if (@value1==1 || @value2==1) goto L_YES;`： 如果 `@value1=1` 或者 `@value2=1` 满足其中一个条件跳转到 `L_YES`
- 示例 `if (@value1==1 && @value2==1) goto L_YES;`： 需要 `@value1=1` 以及 `@value2=1` 同时满足条件跳转到 `L_YES`

常用的比较符号：
- `<`： 小于
- `>`： 大于
- `==`： 等于
- `!=`： 不等于
- `>=`： 大于或等于
- `<=`： 小于或等于

多个条件判断：

- `||`： 或
- `&&`： 和


### end 结束指令

示例 `end;`： 强制结束脚本运行


## 输入输出

### mes 发送消息

- 示例 `mes "你好";`： 在 NPC 对话框中显示 “你好” 
- 示例 `mes @value;`： 在 NPC 对话框中显示 `@value` 的值
- 示例 `mes @value1+@value2;`： 在 NPC 对话框中显示 `@value1+@value2` 的值
- 示例 `mes @value+"你好";`： 在 NPC 对话框中显示 `@value` 的值后加上 “你好”

注意：

- 一行显示一般为 37 个字符，超出的话会自动滚屏增加一行。
- `^123456` 为色彩定义，用 16 进制表示，顺序为红绿蓝三原色，每 2 个定义一种颜色，`FF` 为最亮，`00` 为最暗。
- 色彩定义必须放置在 `""` 之中，而且只能在 `mes` 后的消息中使用。

阅读以下范例，以加强对基本流程及mes语句使用方法的了解：

```
// 发钱的hack
//   里面有几个命令是我们还没有学过的，不过没有关系，很容易理解
//   delitem 2278,1; 表示删除2278号物品1个
//   set Zeny,Zeny+200000; 表示增加200000块钱
//   特别需要注意的是你的 goto 语句，如果出现错误可能会使服务器当机，特别要注意大小写

prontera.gat,155,174,4 script 发钱的hack 706,{
    mes "[发钱的hack]";
    mes "嘿嘿！哇呀呀！";
    mes "我是^5577FF发钱的hack^000000。";
    mes "我愿意拿钱和你换一些东西。";
    next;
    
    menu "询问 ^3355FF笑脸面具^000000 换钱的一些信息",Case1,"拿 ^3355FF笑脸面具^000000 换钱",Case2,"拿 ^3355FF兔耳发圈^000000 换钱",Case4,"取消",Case3;
    Case1:
        mes "[发钱的hack]";
        mes "你有笑脸面具吗？ ";
        mes "我朋友^5577FF乖宝宝^000000想要几个面具来玩。";
        mes "可是我没有材料去制作啊。";
        mes "你有已经制成的面具吗？";
        mes "我可以拿很多钱跟你换哦！";
        next;
        mes "[发钱的hack]";
        mes "制作^3355FF笑脸面具^000000";
        mes "需要搜集一些物品.";
        mes "你如果能搜集到这些材料";
        mes "就可以去找微笑小姐做成笑脸面具";
        mes "下面是制作^3355FF笑脸面具^000000所需要的一些道具.";
        next;
        mes "[发钱的hack]";
        mes "10个^3355FF杰比勒结晶^000000";
        mes "10个^3355FF毛^000000";
        mes "10个^3355FF三叶幸运草^000000";
        next;
        mes "[发钱的hack]";
        mes "做好了拿来给我，我就给你大把大把的钱！ ";
        close;
    Case2:
        if(countitem(2278)<1) goto Case2NOT;
        mes "[发钱的hack]";
        mes "哈哈，^5577FF乖宝宝^000000一定会很高兴的~:)";
        mes "那么^3355FF笑脸面具^000000。我拿走了!.";
        next;
        delitem 2278,1;
        set Zeny,Zeny+200000;
        mes "[发钱的hack]";
        mes "拿好咯！";
        mes "这里是答应你的^5577FF200000Z^000000";
        mes "如果你还有笑脸面具，还可以拿过来和我换。";
        mes "好东西我不嫌多！";
        close;
    Case2NOT:
        mes "[发钱的hack]";
        mes "我";
        next;
        mes "[发钱的hack]";
        mes "要";
        next;
        mes "[发钱的hack]";
        mes "笑";
        next;
        mes "[发钱的hack]";
        mes "脸";
        next;
        mes "[发钱的hack]";
        mes "面";
        next;
        mes "[发钱的hack]";
        mes "具";
        close;
    Case4:
        if(countitem(2214)<1) goto Case4NOT;
        mes "[hack]";
        mes "哈哈，高兴~:)";
        mes "那么^3355FF兔耳发圈^000000。我拿走了!.";
        next;
        delitem 2214,1;
        set Zeny,Zeny+20000000;
        mes "[发钱的hack]";
        mes "拿好咯！";
        mes "这里是答应你的^5577FF20000000Z^000000";
        mes "如果你还有兔耳发圈，还可以拿过来和我换。";
        mes "好东西我不嫌多！";
        close;
    Case4NOT:
        mes "[发钱的hack]";
        mes "我";
        next;
        mes "[发钱的hack]";
        mes "要";
        next;
        mes "[发钱的hack]";
        mes "的";
        next;
        mes "[发钱的hack]";
        mes "是";
        next;
        mes "[发钱的hack]";
        mes "兔";
        next;
        mes "[发钱的hack]";
        mes "耳";
        next;
        mes "[发钱的hack]";
        mes "发";
        next;
        mes "[发钱的hack]";
        mes "圈";
        close;
    Case3:
        mes "[发钱的hack]";
        mes "再见，再见";
        next;
        mes "[发钱的hack]";
        mes "白白，白白";
        close;
}
```


### input 输入命令

示例 `input @value;`： 产生一个输入框让玩家输入，输入的内容赋值在 `@value` 变量中

> 变量的定义方式参考前面【系统变量说明】和【自定义变量说明】


### announce 广播命令

示例 `announce "你好",15;`： 在服务器内用黄色字体广播 “你好”

广播命令，可以控制广播内容和广播颜色字符后的数值可以从 `0-31`，具体参考下表：

- `1`： 表示黄色字体，仅当前地图可见 
- `2`： 表示黄色字体，人物周围范围可见，具体范围大小不明 
- `3`： 表示黄色字体，仅本人可见 
- `0,15`： 效果相同，表示黄色字体，全服务器人可见 
- `17`： 表示蓝色字体，仅当前地图可见 
- `18`： 表示蓝色字体，人物周围范围可见，具体范围大小不明 
- `19`： 表示蓝色字体，仅本人可见 
- `16,31`： 效果相同，表示蓝色字体，全服务器可见


### mapannounce 地图广播

示例 `mapannounce "prontera.gat","你好",15;`： 在普隆德拉地图内内用黄色字体广播 “你好”


### areaannounce 局部广播

区域广播，可以使得特定区域的玩家看到系统消息。

示例 `areaannounce "prontera.gat",100,100,10,10,"你好",3;`： 在普隆德拉地图坐标 `X100,Y100,X10,Y+-10` 的区域内内用黄色字体广播 “你好”。



## 物品相关指令

### getitem "ii" 
getitem 501,1;取得红色药水1个 
道具代码请查看item_db.txt 


### delitem "ii" 
delitem 501,1;删除红色药水1个 
请在执行这条语句前确定是否有该项物品 不然可能发生未知错误 


### itemheal "ii" 
可能与道具损坏有关 
具体用法不明,武器损坏还没开放 


### countitem "i" 
if(countitem(501)==1) goto L_YES; 
如果红色药水数量为1,跳转到L_YES 否则顺序执行 
物品语句统计命令,在()内填入想检查的物品代码 


### getequipname","i" 
getequipname(1); 
按照精练时列出的装备顺序取第1个(头1),以次类推 取得装备名,一般用于判断 


### getequipisequiped "i" 
getequipisequiped(1); 
判断头1是否有装备 序号同上 


### getequipisenableref "i" 
getequipisenableref(1); 
判断头1上的装备是否可精练 序号同上 


### getequipisidentify "i" 
getequipisidentify(1); 
判断头1的装备是否鉴定过 
有些奇怪的命令,没鉴定的装备怎么装备呢 


### getequiprefinerycnt "i" 
if(getequiprefinerycnt(1)<10) goto L_YES; 
如果头1的精练<10,跳转到L_YES 判断装备是否达到预定精练值 


### getequipweaponlv "i" 
if(getequipweaponlv(1)>0) goto L_YES; 
判断头1是否是武器 判断武器等级,小于1为防具,1为1级武器,2为2级武器,依此类推 
30、getequippercentrefinery "i" 
if(getequippercentrefinery(1)==100) goto L_YES; 
判断头1的装备是否到达安定值 
这里需要读取refine_db.txt中的数据以判断是否到达安定值 
在实际使用中,可以用rand(100)得到1个随机数与已知安定值比较以判定是否精练成功 
31、successrefitem "i" 
successrefitem 1; 
将头1装备升1级 装备升级语句 
32、failedrefitem "i" failedrefitem 1; 
将头1装备做精练失败处理 
40、setcart "" 
setcart; 
得到手推车,限定商人铁匠 
41、setfalcon "" 
setfalcon; 
得到猎鹰,限定猎人 
42、setriding "" 
setriding; 
得到骑乘的大嘴鸟或嘟嘟鸟,限定骑士十字军 
44、openstorage "" 
openstorage; 
原地打开仓库 
45、itemskill "iis" 
三个参数分别为技能代码,技能等级,和名称 
46、produce "i" 
制造物品,参数为物品等级, 
具体等级可以查看produce_db.txt 
例1,精炼+20的脚本 
cmd_in01.gat,127,81,8 script 神秘人 731,{ 
mes"[神秘人]"; 
mes"很久没看到猫妹妹了，你有她的照片吗？"; 
next; 
mes"[神秘人]"; 
mes"有的话我可以帮你把物品安全+20"; 
next; 
mes"[神秘人]"; 
mes"说吧！你想要锻治装置项目"; 
mes"中的哪一项物品呢？"; 
next; 
menu getequipname(1),L_MENU_1,getequipname(2),L_MENU_2,getequipname(3),L_MENU_3,getequipname(4),L_MENU_4,getequipname(5),L_MENU_5,getequipname(6),L_MENU_6,getequipname(7),L_MENU_7,getequipname(8),L_MENU_8,getequipname(9),L_MENU_9,getequipname(10),L_MENU_10; 
L_MENU_1: 
set @part,1; 
if (getequipisequiped(1)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_MENU_2: 
set @part,2; 
if (getequipisequiped(2)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_MENU_3: 
set @part,3; 
if (getequipisequiped(3)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_MENU_4: 
set @part,4; 
if (getequipisequiped(4)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_MENU_5: 
set @part,5; 
if (getequipisequiped(5)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_MENU_6: 
set @part,6; 
if (getequipisequiped(6)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_MENU_7: 
set @part,7; 
if (getequipisequiped(7)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_MENU_8: 
set @part,8; 
if (getequipisequiped(8)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_MENU_9: 
set @part,9; 
if (getequipisequiped(9)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_MENU_10: 
set @part,10; 
if (getequipisequiped(10)) goto L_START_1; 
mes"[神秘人]"; 
mes"有毛病啊!"; 
goto L_CLOSE; 
L_START_1: 
if (getequipisenableref(@part)) goto L_START_2; 
mes"[神秘人]"; 
mes"这个项目是无法锻治的。"; 
mes"我想我师傅应该可以"; 
goto L_CLOSE; 
L_START_2: 
if (getequipisidentify(@part)) goto L_START_3; 
mes"[神秘人]"; 
mes"这个还没通过鉴定，无法锻治。"; 
goto L_CLOSE; 
L_START_3: 
if (getequiprefinerycnt(@part) < 20) goto L_REFINE_0; 
mes"[ 神秘人]"; 
mes"还想炼？你有毛病啊！"; 
goto L_CLOSE; 
L_REFINE_0: 
L_0_SUB: 
L_REFINE_1: 
if (countitem(4131) < 1) goto L_CANCEL_2; 
delitem 4131,1; 
L_1_SUB: 
L_REFINE_2: 
L_2_SUB: 
L_REFINE_3: 
L_3_SUB: 
L_REFINE_4: 
L_4_SUB: 
L_REFINE_5: 
mes"[神秘人]"; 
mes" 锵！锵！锵！"; 
mes" 锵！锵！锵！锵！"; 
successrefitem @part; 
mes" 宝刀未老啊！"; 
goto L_CLOSE; 
L_REFINE_6: 
L_CANCEL_1: 
L_CANCEL_2: 
mes"[神秘人]"; 
mes" 拿到照片再来吧！"; 
goto L_CLOSE; 
L_CANCEL_3: 
mes"[神秘人]"; 
mes" ......"; 
L_CLOSE: 
close; 
}



16、viewpoint "iiiii" 
viewpoint 1,134,221,1,0x00ffff; 
以颜色0x00ffff标记小地图上的坐标X134Y221 
43、savepoint "sii" 
savepoint "prontera.gat" 100 100; 
在普隆德拉X100Y100的地点做记录 
存储记录点 
8、warp "sii" 
warp "prontera.gat",100,100;传送至普隆德拉X100Y100的地方 
地图名必须加"" 
9、areawarp "siiiisii" 
areawarp "<origin map>",X0,Y0,X1,Y1,"<destination map>",X,Y 
将X0,Y0-X1,Y1区域的所有玩家都传送到目标地图的X,Y 
72、setmapflagnosave "ssii" 
参数:原地图名,存储地图名,存储X位置,存储Y位置 
例:setmapflagnosave "force_1-1.gat","prontera.gat",156,191 
设置force_1-1.gat地图为不可储存,并把储存点设为prontera.gat,156,191 
73、setmapflag "si" 
功能:设置地图属性(无nosave) 
参数:地图名,属性代码 
属性代码从1开始依次为 
NOMEMO,NOTELEPORT,NOSAVE,NOBRANCH,NOPENALTY, 
PVP,PVP_NOPARTY,PVP_NOGUILD,GVG,GVG_NOPARTY 
PVP和GVG无效果 
74、removemapflag "si" 
功能:取消地图属性(无nosave) 
参数:地图名,属性代码 
用法参考73 
75、pvpon "s" 
pvpon "prontera.gat"; 
开启普隆德拉的即时PK 开启PVP 
76、pvpoff "s" 
pvpoff "prontera.gat"; 
关闭普隆德拉的即时PK 关闭PVP 
77、gvgon "s" 
gvgon "prontera.gat"; 
开启地图工会PK 
78、gvgoff "s" 
gvgoff "prontera.gat"; 
关闭地图工会pk

monster "siisii*" 
monster "prontera.gat" 100 100 "天使波利" 1096 1; 
在普隆德拉X100Y100的地方放出1只天使波利,命名为"天使波利" 
召唤怪物命令,在指定地图指定地点放出指定怪物,可控制召唤数量 
在这里有一个特殊名字"--ja--",定义为mob_db中默认的JName 
48、areamonster "siiiisii*" 
areamonster "prontera.gat" 100 100 10 10 "天使波利" 1096 1; 
在普隆德拉X100Y100,X+-10,Y+-10区域内召唤1只天使波利,命名为"天使波利" 
区域招怪命令 
49、killmonster "ss" 
killmonster "prontera.gat" "s"; 
将普隆德拉地图内所有MOB_DATA为"s"的魔物都杀死 
65、pet "i" 
pet 1002; 
抓取1只宠物波利 
抓取宠物,即获取一个宠物蛋 
66、bpet "" 
孵化一个宠物蛋 
例1,测试怪物专用NPC(取自RO另类) 
//by GeisHa 
prontera.gat,163,176,4 script 测试怪物专用NPC 734,{ 
mes "[测试怪物专用NPC]"; 
mes "我是测试怪物专用NPC"; 
mes "专为大家测试怪物而设立的"; 
next; 
mes "[测试怪物专用NPC]"; 
mes "你想在哪里测试怪物？"; 
next; 
menu "首都西门",L_MENU_1,"首都东门",L_MENU_2,"首都南门",L_MENU_3,"首都北门",L_MENU_4; 
L_MENU_1: 
mes "[测试怪物专用NPC]"; 
mes "请输入你想测试的怪物ID"; 
mes "有效怪物ID为^FF33551001～1392^000000"; 
mes "其中^FF33551324～1363^000000为宝箱"; 
next; 
input a; 
mes "[测试怪物专用NPC]"; 
mes "请输入你想测试的怪物数量"; 
mes "建议输入500，哈哈哈！！！"; 
next; 
input b; 
if (a < 1001) goto L_TSET; 
if (a > 1392) goto L_TSET; 
monster "prt_fild05.gat",338,216,"Monster_Test",a,b,"Monster_Test"; 
next; 
mes "[测试怪物专用NPC]"; 
mes "现在是否要移动过去？"; 
menu "是",-,"不是",L_CLOSE; 
mes "[测试怪物专用NPC]"; 
mes "冲啊!!!"; 
next; 
warp "prt_fild05",367,206; 
close; 
L_MENU_2: 
mes "[测试怪物专用NPC]"; 
mes "请输入你想测试的怪物ID"; 
mes "有效怪物ID为^FF33551001～1392^000000"; 
mes "其中^FF33551324～1363^000000为宝箱"; 
next; 
input a; 
mes "[测试怪物专用NPC]"; 
mes "请输入你想测试的怪物数量"; 
mes "建议输入500，哈哈哈！！！"; 
next; 
input b; 
if (a < 1001) goto L_TSET; 
if (a > 1392) goto L_TSET; 
monster "prt_fild06.gat",55,195,"Monster_Test",a,b,"Monster_Test"; 
next; 
mes "[测试怪物专用NPC]"; 
mes "现在是否要移动过去？"; 
menu "是",-,"不是",L_CLOSE; 
mes "[测试怪物专用NPC]"; 
mes "冲啊!!!"; 
next; 
warp "prt_fild06",31,193; 
close; 
L_MENU_3: 
mes "[测试怪物专用NPC]"; 
mes "请输入你想测试的怪物ID"; 
mes "有效怪物ID为^FF33551001～1392^000000"; 
mes "其中^FF33551324～1363^000000为宝箱"; 
next; 
input a; 
mes "[测试怪物专用NPC]"; 
mes "请输入你想测试的怪物数量"; 
mes "建议输入500，哈哈哈！！！"; 
next; 
input b; 
if (a < 1001) goto L_TSET; 
if (a > 1392) goto L_TSET; 
monster "prt_fild08.gat",170,350,"Monster_Test",a,b,"Monster_Test"; 
next; 
mes "[测试怪物专用NPC]"; 
mes "现在是否要移动过去？"; 
menu "是",-,"不是",L_CLOSE; 
mes "[测试怪物专用NPC]"; 
mes "冲啊!!!"; 
next; 
warp "prt_fild08",170,370; 
close; 
L_MENU_4: 
mes "[测试怪物专用NPC]"; 
mes "请输入你想测试的怪物ID"; 
mes "有效怪物ID为^FF33551001～1392^000000"; 
mes "其中^FF33551324～1363^000000为宝箱"; 
next; 
input a; 
mes "[测试怪物专用NPC]"; 
mes "请输入你想测试的怪物数量"; 
mes "建议输入500，哈哈哈！！！"; 
next; 
input b; 
if (a < 1001) goto L_TSET; 
if (a > 1392) goto L_TSET; 
monster "prt_fild01.gat",201,73,"Monster_Test",a,b,"Monster_Test"; 
next; 
mes "[测试怪物专用NPC]"; 
mes "现在是否要移动过去？"; 
menu "是",-,"不是",L_CLOSE; 
mes "[测试怪物专用NPC]"; 
mes "冲啊!!!"; 
next; 
warp "prt_fild01",200,47; 
close; 
L_TSET: 
mes "[测试怪物专用NPC]"; 
mes "^FF3355你输入的怪物ID出错^000000"; 
mes "有效怪物ID为^FF33551001～1392^000000"; 
mes "其中^FF33551324～1363^000000为宝箱"; 
close; 
L_CLOSE: 
mes "[测试怪物专用NPC]"; 
mes "好吧"; 
close; 
close; 
}

## 其他高级指令
20、rand "i*" 
产生一个随机数 
具体用法不明 
50、doevent "s" 
具体用法不明 
51、addtimer "is" 
添加一个定时器 
第一参数为时间,单位毫秒 
第二参数为事件名,可自定义 
52、deltimer "s" 
删除一个定时器 
当定时器被定义后如果判定不要使用,用此命令删除 
如果不执行删除动作,后果不明 
53、addtimer "si" 
统计定时器 
具体用法不明 
57、getusers "i" 
取得服务器在线玩家人数 
具体用法不明 
58、getmapusers "s" 
getmapusers("prontera.gat"); 
取得普隆德拉地图玩家人数 
可以得到指定地图上的玩家人数 
59、getareausers "siiii" 
getareausers("prontera.gat" 100 100 10 10); 
取得在普隆德拉X100Y100,X+-10,Y+-10的区域内的玩家人数 
待测试 
60、enablenpc "s" 
控制NPC是否出现 
具体用法不明 
61、disablenpc "s" 
控制NPC是否出现 
具体用法不明 
64、debugmes "s" 
debugmes "测试成功"; 
在Map-Server的DOS窗口中输出信息"测试成功" 
Debug输出命令,可以输出字符串或直接使用变量 
例如debugmes @value;将变量@value中的值输出 
70、waitingroom "si*" 
waitingroom "等待比尔" 0; 
开启一个名叫"等待比尔"的聊天室,可容纳人数0 开启聊天室命令,可当作招牌 
71、warpwaitingpc "sii" 
具体用法不明 
例1,艾力克斯的创意 
"我想寫一個NPC， 
跟那人物對話過後他會要玩家去打敗他所召喚出來的怪物， 
如果不幸敗陣了， 
那麼被NPC召喚出來的怪物就會自動消失， 
如果戰勝， 
那剛才召喚怪物的NPC就會自動向你對話並且送給當初和他對話的人一樣禮物， 
而且這個任務不論失敗與否每個人物都只能進行一次。" 
---- 艾力克斯 
//禁止 memo teleport save 
prt_are_in.gat mapflag nomemo dummy 
prt_are_in.gat mapflag noteleport dummy 
prt_are_in.gat mapflag nosave prontera.gat,156,191 
force_1-1.gat mapflag nomemo dummy 
force_1-1.gat mapflag noteleport dummy 
force_1-1.gat mapflag nosave prontera.gat,156,191 
//主NPC,放在普隆XX中间 
prontera.gat,160,180,6 script 艾力克斯 116,{ 
mes "你想要进入艾力克斯的怪物魔境吗?"; //此处可以添加判断,以确定每人只能进一次 
next; 
menu "偶还是宝宝耶,不要打太疼哦!",L_GOARENA1,"偶是猛将兄,警告你哦,不惹我哦!!!",L_GOARENA2,"不要啦,偶怕怕!",L_YAME1; 
L_GOARENA1: 
if( getmapusers("force_1-1.gat")>0 ) goto L_WAIT1; 
disablenpc "ThatsIt"; 
addtimer 5000,"might-test1"; 
disablenpc "艾力克斯"; 
warp "force_1-1.gat",25,26; 
end; 
L_GOARENA2: 
if( getmapusers("force_1-1.gat")>0 ) goto L_WAIT1; 
disablenpc "ThatsIt"; 
addtimer 5000,"might-test3"; 
disablenpc "艾力克斯"; 
warp "force_1-1.gat",25,26; 
end; 
L_WAIT1: 
mes "艾力克斯的怪物魔境有MM在换衣服耶,"; 
mes "不许偷看!!!"; 
L_YAME1: 
close; 
} 
//宝宝级 
force_1-1.gat,25,26,0 script might-test1 -1,{ 
killmonster "force_1-1.gat","might-test2"; 
killmonster "force_1-1.gat","might-test4"; 
monster "force_1-1.gat",25,25,"--ja--",1002,1,"might-test2"; 
monster "force_1-1.gat",20,25,"--ja--",1002,1,"might-test2"; 
monster "force_1-1.gat",25,20,"--ja--",1002,1,"might-test2"; 
monster "force_1-1.gat",30,25,"--ja--",1002,1,"might-test2"; 
monster "force_1-1.gat",25,30,"--ja--",1002,1,"might-test2"; 
set $might00,5; 
} 
//检查怪物是否死光光 
force_1-1.gat,25,26,0 script might-test2 -1,{ 
set $might00, $might00 - 1; 
if( $might00 > 0 ) goto L_CONT1; 
announce "你成功了耶!",3; 
enablenpc "ThatsIt"; 
L_CONT1: 
end; 
} 
//猛将级 
force_1-1.gat,26,27,0 script might-test3 -1,{ 
killmonster "force_1-1.gat","might-test2"; 
killmonster "force_1-1.gat","might-test4"; 
monster "force_1-1.gat",26,27,"--ja--",1147,1,"might-test4"; 
set $might01,1; 
} 
//检查怪物是否死光光 
force_1-1.gat,25,26,0 script might-test4 -1,{ 
set $might01, $might00 - 1; 
if( $might01 > 0 ) goto L_CONT1; 
announce "你成功了耶!",3; 
enablenpc "ThatsIt"; 
L_CONT1: 
end; 
} 
//end of fight 
force_1-1.gat,25,26,6 script ThatsIt 116,{ 
mes "太好了!!!"; 
mes "你成功闯过了Alex的怪物魔境"; 
mes "希望你有一个美好的回忆哦^^"; 
mes "偶来送你回首都吧."; //此处可以添加奖励物品 
next; 
enablenpc "艾力克斯"; 
warp "prontera.gat",156,191; 
close; 
}
一、事件触发及计时指令 

1.doevent 执行某事件 
例1: 
prontera.gat,155,180,0 script ev_doevent 116,{ 
doevent "event_test"; 
} 
prontera.gat,150,185,0 script event_test -1,{ 
announce " 工会战开启!!!",15; 
close; 
} 
event_test的类型定义为了-1，表示是一个事件而不是NPC， 
访问ev_doevent这个NPC之后，将会执行event_test这个事件。 
2.执行召唤monster事件 
例2: 
prontera.gat,150,185,0 script ev_mobevent 116,{ 
monster "this",0,0,"Event_Mob2",1002,1,"event_test"; 
} 
访问这个NPC之后将会召唤出可爱的小波利 
3.将怪杀死后执行某事件 
例3: 
prontera.gat,150,180,0 monster Event_Mob 1008,1,0,0,event_test3 
prontera.gat,150,185,0 script event_test3 -1,{ 
announce " 华丽金属被破坏！",15; 
close; 
} 
4.addtimer 定时器 
用法：addtimer 时间,"事件名::标号" 
注：时间以毫秒为单位，标号必须以On开头 
例4: 
prontera.gat,155,185,0 script ev_timerevent 116,{ 
addtimer 5000,"ev_timerevent::OnTimer"; 
end; 
OnTimer: 
announce "还有5秒钟将关闭工会战",15; 
close; 
} 
5.OnInit NPC初始化事件 
例5: 
prontera.gat,145,180,0 script ev_initevent 116,{ 
end; 
OnInit: 
waitingroom "OnInit测试",1,"ev_initevent::OnMax"; 
end; 
OnMax: 
warpwaitingpc "prontera.gat",155,190; 
end; 
} 
初始化时开启一个聊天室，此处用到了waitingroom的第三个参数， 
仍然是一个事件参数，有兴趣的可以实际测试一下效果。 
6.指定时间执行 
例7: 
prontera.gat,145,185,0 script ev_clockevent 116,{ 
end; 
// 每个小时的第5分钟执行 
OnMinute05: 
announce "每个小时的第5分钟到了！",15; 
end; 
// 每天12点执行 
OnHour12: 
announce "中午到了！！",15; 
end; 
// 23点59分执行 
OnClock2359: 
announce "还有一分钟就要到明天了！！",15; 
end; 
// 2月14日零时执行 
OnDate0214: 
announce "又是一个情人节了，请Kiss旁边最近的MM！！",8; 
end; 
} 
二、事件的写法 
例8: 
// 最简单的事件 
prontera.gat,150,185,0 script event_test -1,{ 
announce strcharinfo(0) + " 召唤怪兽成功！",16; 
close; 
} 
// 仅对范围5格内的有效(待测试) 
prontera.gat,155,180,0 script event_test2 -1,5,5 { 
announce "仅对范围5格内的有效",16; 
close; 
} 
// 效果不明 
prontera.gat,150,180,0 script event_test3 -1,-1,-1 { 
mes "偶不知道什么效果耶！"; 
close; 
} 
三、禁止某些功能 
prontera.gat mapflag nosave prontera.gat,156,190 
禁止存储，把存储点设为prontera.gat,156,190 
prontera.gat mapflag nomemo dummy 
禁止记录(服事系列传送技能) 
prontera.gat mapflag noteleport dummy 
禁止使用苍蝇、蝴蝶，也禁止飞。 
其他设置参看mapflag.txt


标签用于标识一个唯一的行地址。方便脚本程序正确跳转到目标地址继续执行
如使用goto,menu等指令
使用doevent等也可以跳转到指定的NPC中的指定标签行
标签最大长度不能超过22个字符，第23个字符只可以是’:’
特殊标签
OnClock<hour><minute>:
OnHour<hour>:
On<weekday><hour><minute>:
OnDay<month><day>:
以上标签后的内容在时间到的时候自动执行
OnInit:　　　　　　NPC脚本读取完成时执行,用@reloadscript重读脚本也会在脚本读取完后执行
OnInterIfInit:　　　　当map-server与char-server连接时执行
OnInterIfInitOnce:　　只会在map-server与char-server首次连接时执行,之后map-server与char-server重连接也不会再次执行
OnAgitStart:　　　　工会战开始时执行(AgitStart命令执行时)
OnAgitEnd:　　　　工会战结束时执行(AgitEnd命令执行时)
OnAgitInit:　　　　当char-server加载完工会城堡资料到map-server时执行,用@reloadscript重读脚本不会执行
OnAgitEliminate:　　当华金被破坏后,达到’battle_athena.conf’中指定的时间后执行
OnAgitBreak:　　　　当该地图的华金被破坏时执行
因以上特殊标签会在特殊条件下自动执行,没有触发玩家的存在,即没有RID,所以需要RID的指令不能被执行.
OnTouch:
玩家通过触发线触发NPC,而不是点击，如果存在OnTouch标签，则脚本自动从OnTouch标签开始执行，没有则从第一行开始
--------------- EA中特有的特殊标签 ---------------------------
OnPCDieEvent:
OnPCKillEvent:
OnPCLogoutEvent:
OnPCLoginEvent:
OnPCLoadMapEvent:
OnNPCKillEvent:
OnPCBaseUpEvent:
这些标签必须把’script_athena.conf’中的’event_script_type’一项的值设为 1 才能被激活
并且你可以在该文件中修改这些标签的定义
如果’event_script_type’设为0，则这些只能作为NPC事件存在，具体参考下面的脚本脚本
PCDieEvent - 任意玩家死亡时执行，对象为该死亡的玩家
PCKillEvent - 任意玩家被另一玩家杀死时执行，对象为杀人的玩家
　　　　 同时被杀的玩家会被赋给一个特殊的玩家变量’killerrid’记录杀死他的玩家的ID
PCLogoutEvent - 任意玩家离线时执行。在玩家的资料还没从内存中删除之前，对象是该离线的玩家
PCLoadMapEvent - 任意玩家转换地图时执行（该功能好像存在问题，不能正确的取得RID，请大家把测试的结果发表出来）
NPCKillEvent - 任意玩家杀死任意的怪物(包括MVP)时执行，对象是该杀怪的玩家。同时赋给玩家一个特殊的变量’killedrid’记录被杀的怪物编号
　　　　 这个标签不能像上面几个可以在’script_athena.conf’中改名。不确定这个可不可以作为标签被激活。
PCBaseUpEvent - 任意玩家基本等级上升时执行，对象为该升级的玩家
　　　　 这个标签不能像上面几个可以在’script_athena.conf’中改名。不确定这个可不可以作为标签被激活。
以上是所有的特殊标签，这些标签不需要相关的跳转命令就可以被激活。其他标签的编写方式和这些类似，但他们都需要相关的跳转指令才能被执行。
EA中特有的特殊标签作为NPC的方式使用:
// 注意:
// 1) NPC名称为专用的,不能随意修改
// 2) 使用-1使这些NPC不可见,只是因为你没有必要见到他们
// 3) 如果你不需要这些NPC,直接删除即可
// 4) 如果你添加了多个同名的这些NPC,会发生不可预见的事情
// 5) 你可以把这些NPC放到任意的脚本文件中
// 6) 修改这些NPC的内容为你喜欢的内容
// 7) 记住: 这些NPC运行起来和普通的NPC没什么区别,他们只是触发的形式特殊.
prontera.gat,0,0,0　　script　　PCDieEvent　　-1,{
　　mes "你死了!";
close;
end;
}
prontera.gat,1,1,0　　script　　PCKillEvent　　-1,{
　　mes "你杀人了!";
close;
end;
}
prontera.gat,2,2,0　　script　　PCLogoutEvent　　-1,{
　　mes "欢迎下次再来!!";//这个好像看不到,因为已经下线了
close;
end;
}
prontera.gat,3,3,0　　script　　PCLoginEvent　　-1,{
　　mes "欢迎来到咚咚乐园!";
close;
end;
}
prontera.gat,4,4,0　　script　　PCLoadMapEvent　　-1,{
　　mes "你进入了另一幅地图!";
close;
end;
}
prontera.gat,5,5,0　　script　　NPCKillEvent　　-1,{
　　mes "你杀了一只 ["+killedrid+"]";
close;
end;
}
prontera.gat,6,6,0　　script　　PCBaseUpEvent　　-1,{
　　mes "恭喜升级!";
close;
end;
}
--------------- 以上只能在EA中生效 ---------------------------
----------------------------------------
# 关于RID的解释以及为什么要知道什么是RID
----------------------------------------
大部分脚本指令和函数都会需要一个读取关于玩家的数据/存储变量到玩家对象/传送数据到玩家的客户端
这个玩家对象的ID就是RID。
一般RID自动采用触发NPC的玩家的账号ID
如果是写一些公用的NPC,不需要RID.则你要确保你所用到的函数，计时器，指令等可以在没有RID的情况下运行。
否则当指令需要读取/修改指定玩家资料或发送数据到指定玩家的客户端，但却没有RID，程序将不能继续造成服务器停顿或当掉。


//波利赛跑的等待区域
warp quiz_02.gat,300,241 ;






## 常用指令

mes "[message]";
显示消息
[message]:信息内容

next;
进入下个页信息

close;
全部对话结束

menu "[text]",[label],"[text]",[label]???;
选项
[test]：选项的名称
[label]：脚本中行的跳转点。

goto [label];
跳转到脚本的某一行。

cutin "[filename]",[flag];
显示图片，就象我们点击kapula后，会有kapula的一副图片显示在屏幕上。
[filename]：图片名称kafra_01 _ kafra_06 ( comodo 是 07)
[flag]：2显示； 255消失。

store;
仓库

warp "[mapname]",[X],[Y];
地图跳转。
[mapname]:地图名称
[X],[Y]:坐标

save "[mapname]",[X],[Y];
存储纪录
[mapname]:地图名称
[X],[Y]:坐标

heal [HP+],[SP+];
指定HP和SP的恢复量
最大值是 30000

set [formula],[limit];
值的变更
[type][= + += - -=][val]の形式で指定します。
[type]：(Zeny,Job,BaseLevel,JobLevel,StatusPoint,SkillPoint)等自己定义的变量
值的增加用 += ; 如： Zeny += 100,就是说Zeny加100
减少则相反用 -= 。
自己定义变量常用来做任务。
如： set pet_food = 1 ; 自己定义了一个 pet_food 变量，值是1。可以在后面的脚本中来判断这个值。

additem [ItemID],[amount];
道具添加
[ItemID]编号
[Amount]个数

delitem [ItemID],[amount];
删除道具
[ItemID]编号
[Amount]个数

check [formula],[label1],[label2];
对应上面的自定义变量
[formula]变量名称
[type][<= >= = == <> != > <][val]
[type](Zeny,Job,BaseLevel,JobLevel,StatusPoint,SkillPoint)等自定义变量。
判断后结果正确，跳转到[label1]
不成立，跳转到[label2]
Example:

check Pet_Food > 0,L_CHECK_1,-; //检查 Pet_Food 这个自定义变量，如果>0，跳转到L_CHECK_1，不大于0继续。
mes "[Egar]";
mes "Can I help you?";
close;
L_CHECK_1:
mes "[Egar]";
mes "give you";
additem 541,1; //这里，添加道具了。
next;

jobchange [job];
职业的变更。
[job]：数字
0 初心者
1 劍士
2 法師
3 弓箭手
4 牧師
5 商人
6 盜贼
7 騎士
8 祭師
9 巫師
10 鐵匠
11 獵人
12 刺客
14十字军
15武僧
16贤者
17流氓
18练金术士
19诗人
20舞者


viewpoint [type],[X],[Y],[PointID],[color];
标记某个坐标，在地图上的该坐标点用[color]显示。
[type]：1表示显示，2表示影藏。
[X]，[Y]：点的坐标。
[PointID]：坐标编号，如：1代表道具店，2代表武器店。这样在地图上就能同时显示两个点。
如果都用1的话，地图上只能显示一个点。
[color]：颜色，用16进制表示。如： 0xFF0000， 0x 是表示16进制，后面的FF0000是颜色代码。
拥有[ItemID]所指定物品[Amount]个或以上数量的话
到[label1]，反之到[label2]
如果要调查的物品为装备品的话，请务必将[amount]设为1
得到[ItemID]所指定的道具[Amount]个的话
如果重量超过最大值就转到[label1]
反之即转到[label2]
如果要调查的物品为装备品的话，请务必将[amount]设为1.这个指令是配合additem的指令