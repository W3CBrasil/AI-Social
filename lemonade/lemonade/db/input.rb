# Arquivo responsavel por popular template input no banco
#
#
#
def delete
  Template.delete_all(category: 'input')
  Interface.delete_all(category: 'input')
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

def pop_input

  templates = []
  interface = []

  #CSV input
  templates <<  { name:'csv_in',
                  display_name:'CSV',
                  category: 'input',
                  parameters: {
                    text: { "Step name": 'csv', delimiter: ',', enclosure:'"'},
                    select: {encode: { selected:'UTF-8', list:['UTF-8', 'latin']}},
                    check_box: { header:'0'},
                    browser: { File: '/'}
                  }
  }
  interface << { name:'csv_in',
                 display_name: 'CSV',
                 position: { x: 350, y: 300},
                 size: { width: 80, height: 80 },
                 category: 'input',
                 outPorts: [''],
                 attrs: {
                   '.body': { stroke: ' #408000'},
                     '.label': {text: 'CSV', fill:' #408000'},
                     '.port-body': { fill: '#66cc00'}}
    }

    delete
    pop_templates templates
    pop_interface interface
  end



