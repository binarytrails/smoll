import sys

# Example:
# $ wget http://example.com
# $ grep -aoE "https?://[a-zA-Z0-9./?=_-]+" index.html | grep -i 'iana\.org' | sort -u | python3 url_to_tree.py
# domains/
#     example/ 

def print_tree(paths):
    tree = {}
    for path in paths:
        parts = path.strip().split('/')
        node = tree
        for part in parts:
            if part not in node:
                node[part] = {}
            node = node[part]

    def print_node(node, prefix=''):
        for key, subnode in node.items():
            print(f"{prefix}{key}/")
            if isinstance(subnode, dict):
                print_node(subnode, prefix + '    ')

    print_node(tree)

if __name__ == "__main__":
    urls = (line.strip() for line in sys.stdin)
    paths = (url.split('//')[1].split('/', 1)[1] if '/' in url.split('//')[1] else '' for url in urls)
    print_tree(paths)
