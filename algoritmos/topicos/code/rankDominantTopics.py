import sys
import operator

# Script chamado por run_all.py
# Le a saida do LDA e gera os arquivos de topic_rank e document_topic
inputFolder = sys.argv[1]
wordsFile = open( "%s/model-final.twords" % inputFolder,'r')
docTopicsFile = open( "%s/model-final.theta" % inputFolder,'r')

rankFile = open("topic_rank.txt", 'w')
topicDocumentFile = open("document_topic.txt",'w')

topicsWords = []
topicsWordsValues = []

i = -1
for line in wordsFile:
  line = line.strip()

  if line.startswith("Topic ") and line.endswith("th:"):
    i = i+1
    topicsWords.append( [] )
    topicsWordsValues.append( [] )


  else:
    line = line.strip().split()
    topicsWords[i].append(line[0])
    topicsWordsValues[i].append(line[1])


topicRank = {}

print >> topicDocumentFile, "//topicId\ttopicWords"
for line in docTopicsFile:
  values = map( float, line.strip().split() )

  maxValue = max(values)
  maxTopic = values.index(maxValue)

  print >> topicDocumentFile, "%s\t%s" % ( maxTopic, ",".join(topicsWords[maxTopic]) )

  if maxTopic not in topicRank:
    topicRank[maxTopic] = 0
    
  topicRank[maxTopic] += 1

rank = sorted( topicRank.items(), key = operator.itemgetter(1), reverse=True )

print >> rankFile, "//topicID\t#numberOfDocs\tTopicWords"
for topic,importance in rank:
  print >> rankFile, "%s\t%s\t%s" % (topic, importance, ",".join(topicsWords[topic]) )
