# Arquivo responsavel por popular template Elasticsearch no banco
#
#
#
def pop_templates templates
  templates.each do |value|
    Template.create(value)
  end
end

def pop_interface interface
  interface.each do |value|
    Interface.create(value)
  end
end

def delete_elastic
  Template.delete_all(category: 'elastic')
  Interface.delete_all(category: 'elastic')
end

def pop_elastic

templates = []
interface = []

#Input
templates <<  { name:'in_db_elastic',
                 display_name:'Datasets',
                 category: 'elastic',
                 parameters: {
                   text: { "Step name": 'input'},
                   select: {"Datasets":{ selected: 'dados', list: ['dados_1', 'dados_2']}},
                   check_box: {},
                   browser: {}
                 }
}

interface << { name:'in_db_elastic',
                display_name: 'Datasets',
                position: { x: 350, y: 300},
                size: { width: 80, height: 80 },
                category: 'elastic',
                outPorts: ['out'],
                attrs: {
                  '.port-label': { fill: '#ffffff'},
                  '.body': { stroke: '#5a300c'},
                  '.label': {text: 'Input', fill:'#5a300c'},
                  '.outPorts circle': { fill: '#9d5515', type: 'output' }
                }
}

#filter
templates <<  { name:'filter_elastic',
                 display_name:'Filter',
                 category: 'elastic',
                 parameters: {
                   text: { "Step name": 'Filter', "query": 'comment_text:PALAVRA'},
                   select: {},
                   check_box: {},
                   browser: {}
                 }
}

interface << { name:'filter_elastic',
                display_name: 'Filter',
                position: { x: 350, y: 300},
                size: { width: 80, height: 80 },
                category: 'elastic',
                inPorts: ['in'],
                outPorts: ['out'],
                attrs: {
                  '.port-label': { fill: '#ffffff'},
                  '.body': { stroke: '#5a300c'},
                  '.label': {text: 'Filter', fill:'#5a300c'},
                  #'.port-body': { fill: '#9d5515'},
                  '.inPorts circle': { fill: '#9d5515', magnet: 'passive', type: 'input' },
                  '.outPorts circle': { fill: '#9d5515', type: 'output' }
                }
}

# Sentiment Analysis
templates <<  { name:'sentiment_analysis_elastic',
                 display_name:'Sentiment Analysis',
                 category: 'elastic',
                 parameters: {
                   text: { "Step name": 'Sentiment Analysis'},
                   select: {},
                   check_box: {},
                   browser: {}
                 }
}

interface << { name:'sentiment_analysis_elastic',
                display_name: 'Sentiment Analysis',
                position: { x: 350, y: 300},
                size: { width: 80, height: 80 },
                category: 'elastic',
                inPorts: ['in'],
                outPorts: ['out'],
                attrs: {
                  '.port-label': { fill: '#ffffff'},
                  '.body': { stroke: '#5a300c'},
                  '.label': {text: 'Sentiment Analysis', fill:'#5a300c'},
                  #'.port-body': { fill: '#9d5515'},
                  '.inPorts circle': { fill: '#9d5515', magnet: 'passive', type: 'input' },
                  '.outPorts circle': { fill: '#9d5515', type: 'output' }
                }
}

#Correlation Patern
templates <<  { name:'correlation_patern_elastic',
                 display_name:'Correlation Patern',
                 category: 'elastic',
                 parameters: {
                   text: { "Step name": 'Correlation'},
                   select: {},
                   check_box: {},
                   browser: {}
                 }
}

interface << { name:'correlation_patern_elastic',
                display_name: 'Correlation Patern',
                position: { x: 350, y: 300},
                size: { width: 80, height: 80 },
                category: 'elastic',
                inPorts: ['in'],
                outPorts: ['out'],
                attrs: {
                  '.port-label': { fill: '#ffffff'},
                  '.body': { stroke: '#5a300c'},
                  '.label': {text: 'Correlation', fill:'#5a300c'},
#                  '.port-body': { fill: '#9d5515'},
                  '.inPorts circle': { fill: '#9d5515', magnet: 'passive', type: 'input' },
                  '.outPorts circle': { fill: '#9d5515', type: 'output' }


                }
}

#
#endorsement

templates <<  { name:'endorsement_elastic',
                display_name:'endorsement',
                 category: 'elastic',
                 parameters: {
                   text: { "Step name": 'endorsement'},
                   select: {},
                   check_box: {},
                   browser: {}
                 }
}

interface << { name:'endorsement_elastic',
               display_name: 'endorsement',
                position: { x: 350, y: 300},
                size: { width: 80, height: 80 },
                category: 'elastic',
                inPorts: ['in'],
                outPorts: ['out'],
                attrs: {
                  '.port-label': { fill: '#ffffff'},
                  '.body': { stroke: '#5a300c'},
                  '.label': {text: 'endorsement', fill:'#5a300c'},
#                  '.port-body': { fill: '#9d5515'},
                  '.inPorts circle': { fill: '#9d5515', magnet: 'passive', type: 'input' },
                  '.outPorts circle': { fill: '#9d5515', type: 'output' }

                }
}

#Topics

templates <<  { name:'topics_elastic',
                 display_name:'Topics',
                 category: 'elastic',
                 parameters: {
                   text: { "Step name": 'Topics',"n_words":'10', "n_topics": '50', "n_iters": '1000'},
                   select: {},
                   check_box: {},
                   browser: {}
                 }
}

interface << { name:'topics_elastic',
                display_name: 'Topics',
                position: { x: 350, y: 300},
                size: { width: 80, height: 80 },
                category: 'elastic',
                inPorts: ['in'],
                outPorts: ['out'],
                attrs: {
                  '.port-label': { fill: '#ffffff'},
                  '.body': { stroke: '#5a300c'},
                  '.label': {text: 'topics', fill:'#5a300c'},
#                  '.port-body': { fill: '#9d5515'},
                  '.inPorts circle': { fill: '#9d5515', magnet: 'passive', type: 'input' },
                  '.outPorts circle': { fill: '#9d5515', type: 'output' }
                }
}

#output
templates <<  { name:'output_elastic',
                 display_name:'Output',
                 category: 'elastic',
                 parameters: {
                   text: { "Step name": 'Output'},
                   select: {},
                   check_box: {},
                   browser: {}
                 }
}

interface << { name:'output_elastic',
                display_name: 'Output',
                position: { x: 350, y: 300},
                size: { width: 80, height: 80 },
                category: 'elastic',
                inPorts: ['in'],
                attrs: {
                  '.port-label': { fill: '#ffffff'},
                  '.body': { stroke: '#5a300c'},
                  '.label': {text: 'output', fill:'#5a300c'},
                  #'.port-body': { fill: '#9d5515'},
                  '.inPorts circle': { fill: '#9d5515', magnet: 'passive', type: 'input' }                }
}

delete
pop_templates templates
pop_interface interface
end
