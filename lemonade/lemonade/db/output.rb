# Arquivo responsavel por popular template output no banco
#
#
#
def delete
  Template.delete_all(category: 'output')
  Interface.delete_all(category: 'output')
end



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

def pop_output

  templates = []
  interface = []

  #CSV output
  templates <<  { name:'csv_out',
                  display_name:'CSV',
                  category: 'output',
                  parameters: {
                    text: { "Step name": 'csv', delimiter: ',', enclosure:'"'},
                    select: {encode: { selected:'UTF-8', list:['UTF-8', 'latin']}},
                    check_box: { header:'0'},
                    browser: { File: '/'}
                  }
  }
  interface << { name:'csv_out',
                 display_name: 'CSV',
                 position: { x: 350, y: 300},
                 size: { width: 80, height: 80 },
                 category: 'output',
                 inPorts: [''],
                 attrs: {
                   '.body': { stroke: '#0059b3'},
                     '.label': {text: 'CSV', fill:'#0059b3'},
                     '.port-body': { fill: '#0080ff'}}
    }

#    delete
    pop_templates templates
    pop_interface interface
  end



