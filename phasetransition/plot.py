from glob import glob

def filename2beta(fn):
    return float(fn.split('_')[2])

if __name__ == "__main__":
    bs = ["0.010000", "0.005000", "0.001000", "0.000500", "0.000100"]
    for b in bs:
        fns = glob(f'*_{b}_*.merror')
        fns.sort()
        data = []
        for fn in fns:
            beta = filename2beta(fn)
            if beta == 0:
                continue
            print(fn)
            with open(fn, 'r') as fd:
                words = fd.read().split('\t')
            mean = float(words[0]) / (100**2)
            error = abs(float(words[1])) / (100**2)
            data.append((1 / beta, mean, error))
        data.sort(key=lambda x: x[0])
        with open(f"{b}_magnetisation2.plt", "w") as fd:
            for d in data:
                fd.write(f"{d[0]}\t{d[1]}\t{d[2]}\n")
