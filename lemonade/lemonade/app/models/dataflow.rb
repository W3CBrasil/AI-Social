class Dataflow
  include Mongoid::Document
  include Mongoid::Attributes::Dynamic
  include Mongoid::Timestamps

  validates_presence_of :name

  belongs_to :user

  field :name, type: String
  field :last_result, type: String
  field :interface
#  field :box, type: BSON::Document
  field :log, type: BSON::Document

  has_many :box

  def draw_flow
    links = []
    tree = {}
    sequence = []
    JSON.parse(interface)["cells"].each do |cell|
      if cell["type"] == "devs.Box"
        id = cell['id']
        tree[id] = []
      elsif cell["type"] == "link"
        links << cell
      end
    end
    links.each do |link|
      tree[link['source']['id']] << link['target']['id']
    end
    tree.tsort.reverse.each do |id|
      sequence << box.find(id)
    end

    [sequence, tree]
  end
end
