#
# This ProGuard configuration file illustrates how to process ProGuard itself.
# Configuration files for typical applications will be very similar.
# Usage:
#     java -jar proguard.jar @proguard.pro
#



-injars       SCO_Verify.jar
-outjars      ProGuard_SCO_Verify.jar
-libraryjars  C:/Program Files/Java/jdk1.8.0_202/jre/lib/resources.jar
-libraryjars  C:/Program Files/Java/jdk1.8.0_202/jre/lib/rt.jar
-libraryjars  C:/Program Files/Java/jdk1.8.0_202/jre/lib/jsse.jar
-libraryjars  C:/Program Files/Java/jdk1.8.0_202/jre/lib/jce.jar
-libraryjars  C:/Program Files/Java/jdk1.8.0_202/jre/lib/charsets.jar
-libraryjars  C:/Program Files/Java/jdk1.8.0_202/jre/lib/ext/dnsns.jar
-libraryjars  C:/Program Files/Java/jdk1.8.0_202/jre/lib/ext/localedata.jar
-libraryjars  C:/Program Files/Java/jdk1.8.0_202/jre/lib/ext/sunjce_provider.jar
-libraryjars  D:/SCO/SCO_Verify/JAR/bcprov-jdk16-1.46.jar
-libraryjars  D:/SCO/SCO_Verify/JAR/jna-3.2.5.jar
-libraryjars  D:/SCO/SCO_Verify/JAR/jpbc-api-1.2.1.jar
-libraryjars  D:/SCO/SCO_Verify/JAR/jpbc-benchmark-1.2.1.jar
-libraryjars  D:/SCO/SCO_Verify/JAR/jpbc-crypto-1.2.1.jar
-libraryjars  D:/SCO/SCO_Verify/JAR/jpbc-pbc-1.2.1.jar
-libraryjars  D:/SCO/SCO_Verify/JAR/jpbc-plaf-1.2.1.jar

-printmapping proguard.map
-overloadaggressively
-repackageclasses ''
-allowaccessmodification
-keep class jaas.Main{*;}
