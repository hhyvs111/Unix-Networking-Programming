#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
 
 
int main (int argc , char **argv)
{
xmlDocPtr pdoc = NULL;
xmlNodePtr proot = NULL, pcur = NULL;
/*****************��xml�ĵ�********************/
xmlKeepBlanksDefault(0);//������ϣ���ֹ�����Ԫ��ǰ��Ŀհ��ı����ŵ���һ��node
pdoc = xmlReadFile ("test.xml", "UTF-8", XML_PARSE_RECOVER);//libxmlֻ�ܽ���UTF-8��ʽ����
 
if (pdoc == NULL)
{
printf ("error:can't open file!\n");
exit (1);
}
 
/*****************��ȡxml�ĵ�����ĸ��ڶ���********************/
proot = xmlDocGetRootElement (pdoc);
 
if (proot == NULL)
{
printf("error: file is empty!\n");
exit (1);
}
 
/*****************��������������鼮������********************/
pcur = proot->xmlChildrenNode;
 
while (pcur != NULL)
{
//��ͬ��׼C�е�char����һ����xmlCharҲ�ж�̬�ڴ���䣬�ַ��������� ��غ���������xmlMalloc�Ƕ�̬�����ڴ�ĺ�����xmlFree�����׵��ͷ��ڴ溯����xmlStrcmp���ַ����ȽϺ����ȡ�
//����char* ch="book", xmlChar* xch=BAD_CAST(ch)����xmlChar* xch=(const xmlChar *)(ch)
//����xmlChar* xch=BAD_CAST("book")��char* ch=(char *)(xch)
if (!xmlStrcmp(pcur->name, BAD_CAST("book")))
{
xmlNodePtr nptr=pcur->xmlChildrenNode;
while (pcur != NULL)
{
if (!xmlStrcmp(nptr->name, BAD_CAST("title")))
{
printf("title: %s\n",((char*)XML_GET_CONTENT(nptr->xmlChildrenNode)));
break;
}
}
 
}
pcur = pcur->next;
}
 
/*****************�ͷ���Դ********************/
xmlFreeDoc (pdoc);
xmlCleanupParser ();
xmlMemoryDump ();
return 0;
}