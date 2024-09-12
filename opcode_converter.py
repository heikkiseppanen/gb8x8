import json

with open('Opcodes.json', 'r') as file:
    data = json.load(file)

def print_op(op):
    mnemonic = op['mnemonic']
    bytes = op['bytes']
    cycles = op['cycles'][0] // 4

    operand1 = f"\"${op['operands'][0]['name']}\"" if len(op['operands']) > 0 else 'NULL'
    operand2 = f"\"${op['operands'][1]['name']}\"" if len(op['operands']) > 1 else 'NULL'

    print(f"create_operation(\"{mnemonic}\", {bytes}, {cycles}, {operand1}, {operand2}),")

for key in data['unprefixed']:
    op = data['unprefixed'][key]
    print_op(op)

for key in data['cbprefixed']:
    op = data['cbprefixed'][key]
    print_op(op)