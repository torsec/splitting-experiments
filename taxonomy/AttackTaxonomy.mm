<map version="1.0.1">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node CREATED="1488445804591" ID="ID_645754657" MODIFIED="1488445829583" TEXT="Attack Taxonomy">
<node CREATED="1488445845780" ID="ID_781472182" MODIFIED="1488445853241" POSITION="right" TEXT="Obstacle">
<node CREATED="1488445863186" ID="ID_1735614071" MODIFIED="1488445868123" TEXT="Protection">
<node CREATED="1488445868124" ID="ID_1972103182" MODIFIED="1488445877548" TEXT="Obfuscation">
<node CREATED="1488445878269" ID="ID_967694482" MODIFIED="1488445882155" TEXT="Control flow"/>
<node CREATED="1488445882837" ID="ID_215958069" MODIFIED="1488445886643" TEXT="Opaque predicate"/>
</node>
<node CREATED="1488445890082" ID="ID_788156717" MODIFIED="1488445895035" TEXT="Anti debugging"/>
<node CREATED="1488445896572" ID="ID_517085226" MODIFIED="1488445906066" TEXT="White box cryptography"/>
</node>
<node CREATED="1488445907516" ID="ID_1058377714" MODIFIED="1488445916882" TEXT="Execution environment">
<node CREATED="1488445917066" ID="ID_664753576" MODIFIED="1488445925457" TEXT="Limitation from OS"/>
</node>
<node CREATED="1488445929435" ID="ID_1662867358" MODIFIED="1488445935634" TEXT="Tool limitations"/>
</node>
<node CREATED="1488445939802" ID="ID_985772664" MODIFIED="1488445949049" POSITION="right" TEXT="Analysis / reverse engineering">
<node CREATED="1488445973946" ID="ID_353719341" MODIFIED="1488445981744" TEXT="String / name analysis"/>
<node CREATED="1488445983073" ID="ID_1554127711" MODIFIED="1488445993240" TEXT="Symbolic execution / SMT solving"/>
<node CREATED="1488445995143" ID="ID_1326195736" MODIFIED="1488445998888" TEXT="Crypto analysis"/>
<node CREATED="1488446000201" ID="ID_1388341055" MODIFIED="1488446004088" TEXT="Pattern matching"/>
<node CREATED="1488446004486" ID="ID_1509918728" MODIFIED="1488449016295" TEXT="Static analysis"/>
<node CREATED="1488446007921" ID="ID_259811585" MODIFIED="1488446011744" TEXT="Dynamic analysis">
<node CREATED="1488446027999" ID="ID_618768018" MODIFIED="1488446036774" TEXT="Dependency analysis">
<node CREATED="1488446037000" ID="ID_853273316" MODIFIED="1488446044151" TEXT="Data flow analysis"/>
</node>
<node CREATED="1488446048532" ID="ID_1594267900" MODIFIED="1488446051807" TEXT="Memory dump"/>
<node CREATED="1488446052228" ID="ID_1064168609" MODIFIED="1488446072992" TEXT="Monitor public interfaces"/>
<node CREATED="1488446073493" ID="ID_537068127" MODIFIED="1488446079774" TEXT="Profiling"/>
<node CREATED="1488446081879" ID="ID_1306349576" MODIFIED="1488446084702" TEXT="Tracing"/>
<node CREATED="1488446092879" ID="ID_610235344" MODIFIED="1488446098661" TEXT="Statistical analysis">
<node CREATED="1488446098967" ID="ID_1559414564" MODIFIED="1488446107141" TEXT="Differential data analysis"/>
<node CREATED="1488446107687" ID="ID_1935289958" MODIFIED="1488446116627" TEXT="Correlation analysis"/>
</node>
</node>
<node CREATED="1488446117134" ID="ID_319838075" MODIFIED="1488446121397" TEXT="Black box analysis">
<node CREATED="1488446121774" ID="ID_1942193775" MODIFIED="1488446126589" TEXT="File format analysis"/>
</node>
</node>
<node CREATED="1488446565602" ID="ID_956856823" MODIFIED="1488446573432" POSITION="right" TEXT="Workaround"/>
<node CREATED="1488446574048" ID="ID_723146797" MODIFIED="1488446579937" POSITION="right" TEXT="Weakness">
<node CREATED="1488446667557" ID="ID_1781452972" MODIFIED="1488446673910" TEXT="Global pointer table"/>
<node CREATED="1488446674526" ID="ID_462881990" MODIFIED="1488446682885" TEXT="Recognizable library">
<node CREATED="1488446683113" ID="ID_1248876824" MODIFIED="1488446686790" TEXT="Shared library"/>
<node CREATED="1488446687451" ID="ID_1532757978" MODIFIED="1488446692317" TEXT="Java library"/>
</node>
<node CREATED="1488446695159" ID="ID_853723657" MODIFIED="1488446704141" TEXT="Decrypt code before execution"/>
<node CREATED="1488446704742" ID="ID_349027744" MODIFIED="1488446707949" TEXT="Clear key"/>
<node CREATED="1488446709131" ID="ID_1498954738" MODIFIED="1488452161182" TEXT="Clues available in plain text"/>
<node CREATED="1488446716662" ID="ID_97222301" MODIFIED="1488446719589" TEXT="Clear data in memory"/>
</node>
<node CREATED="1488446585462" ID="ID_1848936551" MODIFIED="1488446588552" POSITION="right" TEXT="Asset"/>
<node CREATED="1488446589265" ID="ID_1318651432" MODIFIED="1488446593616" POSITION="right" TEXT="Background knowledge">
<node CREATED="1488446630564" ID="ID_390468741" MODIFIED="1488446656886" TEXT="Knowledge on the execution environment framework"/>
</node>
<node CREATED="1488446137350" ID="ID_323740673" MODIFIED="1488446143436" POSITION="left" TEXT="Attack strategy"/>
<node CREATED="1488446143926" ID="ID_938514898" MODIFIED="1488446148436" POSITION="left" TEXT="Attack step">
<node CREATED="1488446153157" ID="ID_1002823853" MODIFIED="1488446160620" TEXT="Prepare environment"/>
<node CREATED="1488446161989" ID="ID_1020480072" MODIFIED="1488446175923" TEXT="Reverse engineer app and protections">
<node CREATED="1488446177073" ID="ID_255903597" MODIFIED="1488446187108" TEXT="Understand the app">
<node CREATED="1488446187109" ID="ID_1087690356" MODIFIED="1488446201004" TEXT="Preliminary understanding of the app">
<node CREATED="1488446201005" ID="ID_1831515816" MODIFIED="1488446208646" TEXT="Identify input / data format"/>
</node>
<node CREATED="1488446216403" ID="ID_1917056827" MODIFIED="1488446233052" TEXT="Recognize anomalous / unexpected behavior"/>
<node CREATED="1488446233807" ID="ID_1960986677" MODIFIED="1488446240122" TEXT="Identify API calls"/>
<node CREATED="1488446241200" ID="ID_1159742532" MODIFIED="1488448642331">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Understand persistent storage / file / socket
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1488446255701" ID="ID_1527280087" MODIFIED="1488446260065" TEXT="Understand code logic"/>
</node>
<node CREATED="1488446263529" ID="ID_925732220" MODIFIED="1488446271112" TEXT="Idenitfy sensitive asset">
<node CREATED="1488446273222" ID="ID_148877388" MODIFIED="1488448540764">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Identify code containing&#160;&#160;sensitive asset
    </p>
  </body>
</html>
</richcontent>
<font NAME="Verdana" SIZE="12"/>
<node CREATED="1488446343643" ID="ID_717976687" MODIFIED="1488446352804" TEXT="Identify assets by static meta info">
<node CREATED="1488446352805" ID="ID_1085458355" MODIFIED="1488446363864" TEXT="Identify assets by naming scheme"/>
</node>
</node>
</node>
<node CREATED="1488446367829" ID="ID_1613748896" MODIFIED="1488448514966">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Identify points of attack
    </p>
  </body>
</html>
</richcontent>
<node CREATED="1488446376010" ID="ID_154032982" MODIFIED="1488446386492" TEXT="Identify output generation"/>
</node>
<node CREATED="1488446392447" ID="ID_531615114" MODIFIED="1488446396613" TEXT="Identify protection"/>
<node CREATED="1488446397186" ID="ID_703712406" MODIFIED="1488446404197" TEXT="Run analysis"/>
<node CREATED="1488446406051" ID="ID_1107237252" MODIFIED="1488446411014" TEXT="Reverse engineer the code">
<node CREATED="1488446412690" ID="ID_20585063" MODIFIED="1488446419037" TEXT="Disassemble the code"/>
<node CREATED="1488446419518" ID="ID_123831189" MODIFIED="1488446425925" TEXT="Deobfuscate the code"/>
</node>
</node>
<node CREATED="1488446430945" ID="ID_1787621655" MODIFIED="1488446443164" TEXT="Build attack strategy">
<node CREATED="1488446448803" ID="ID_611061960" MODIFIED="1488446463955" TEXT="Evaluate and select alternative steps / revise strategy">
<node CREATED="1488446466144" ID="ID_1219141408" MODIFIED="1488446473171" TEXT="Chose path of less resistance"/>
</node>
<node CREATED="1488446476543" ID="ID_1303955591" MODIFIED="1488446482515" TEXT="Limit scope of attack">
<node CREATED="1488446482986" ID="ID_335330224" MODIFIED="1488446498115" TEXT="Limit scope of attack by meta info"/>
</node>
</node>
<node CREATED="1488446512634" ID="ID_1198865815" MODIFIED="1488446521730" TEXT="Prepare attack">
<node CREATED="1488446789399" ID="ID_157955688" MODIFIED="1488446798027" TEXT="Chose/evaluate alternative tool"/>
<node CREATED="1488446798474" ID="ID_178146524" MODIFIED="1488446853032">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Customize/extend tool
    </p>
  </body>
</html>
</richcontent>
<node CREATED="1488446813775" ID="ID_891654608" MODIFIED="1488446836586" TEXT="Port tool to target execution environment"/>
</node>
<node CREATED="1488446805568" ID="ID_1379414828" MODIFIED="1488446862585" TEXT="Create new tool for attack"/>
<node CREATED="1488446865351" ID="ID_1614387289" MODIFIED="1488446871647" TEXT="Customize execution environment"/>
<node CREATED="1488446873377" ID="ID_241829408" MODIFIED="1488446877425" TEXT="Build a workaround"/>
<node CREATED="1488446880255" ID="ID_1943589836" MODIFIED="1488446888520" TEXT="Recreate protection in the small"/>
<node CREATED="1488446889213" ID="ID_426423599" MODIFIED="1488446898224" TEXT="Assess effort"/>
</node>
<node CREATED="1488446522478" ID="ID_1282253690" MODIFIED="1488446535747" TEXT="Tamper with code and execution">
<node CREATED="1488446902868" ID="ID_1607075628" MODIFIED="1488446914792" TEXT="Tamper with execution environment"/>
<node CREATED="1488446917062" ID="ID_1852149590" MODIFIED="1488446921592" TEXT="Run app in emulator"/>
<node CREATED="1488446922853" ID="ID_238381202" MODIFIED="1488446927431" TEXT="Undo protection">
<node CREATED="1488446927953" ID="ID_1561689841" MODIFIED="1488446934815" TEXT="Deobfuscate code"/>
<node CREATED="1488446935480" ID="ID_1119705130" MODIFIED="1488446942647" TEXT="Convert code to standard format"/>
<node CREATED="1488446943268" ID="ID_850464311" MODIFIED="1488446953127" TEXT="Disable anti-debugging"/>
<node CREATED="1488446954720" ID="ID_401372318" MODIFIED="1488446969854" TEXT="Obtain clear code after code decryption at runtime"/>
</node>
<node CREATED="1488446973386" ID="ID_1920288272" MODIFIED="1488446983646" TEXT="Tamper with execution">
<node CREATED="1488446983959" ID="ID_423817594" MODIFIED="1488446994301" TEXT="Replace API function with reimplementation"/>
<node CREATED="1488446994798" ID="ID_245273656" MODIFIED="1488446999021" TEXT="Tamper with data"/>
</node>
<node CREATED="1488447007255" ID="ID_320411937" MODIFIED="1488447013214" TEXT="Tamper with code statically"/>
<node CREATED="1488447015942" ID="ID_980768190" MODIFIED="1488447021013" TEXT="Out of context execution"/>
<node CREATED="1488447021618" ID="ID_1213727973" MODIFIED="1488447025597" TEXT="Brute force attack"/>
</node>
<node CREATED="1488446538879" ID="ID_1179150144" MODIFIED="1488446547529" TEXT="Analyze attack results">
<node CREATED="1488447033405" ID="ID_1612514075" MODIFIED="1488447054116" TEXT="Make hypothesis">
<node CREATED="1488447056653" ID="ID_429950978" MODIFIED="1488447063379" TEXT="Make hypothesis on protection"/>
<node CREATED="1488447063801" ID="ID_1366570006" MODIFIED="1488447078310" TEXT="Make hypothesis on reasons for attack failure"/>
</node>
<node CREATED="1488447081243" ID="ID_1007007669" MODIFIED="1488447087259" TEXT="Confirm hypothesis"/>
</node>
</node>
<node CREATED="1488446599569" ID="ID_1761810648" MODIFIED="1488446601224" POSITION="left" TEXT="Tool">
<node CREATED="1488446603580" ID="ID_400655478" MODIFIED="1488446606447" TEXT="Debugger"/>
<node CREATED="1488446607047" ID="ID_85624229" MODIFIED="1488446610560" TEXT="Profiler"/>
<node CREATED="1488446612391" ID="ID_1057387054" MODIFIED="1488446616696" TEXT="Tracer"/>
<node CREATED="1488446617196" ID="ID_1950930592" MODIFIED="1488446619223" TEXT="Emulator"/>
</node>
</node>
</map>
