#!/usr/bin/env python
"""Execute the tests for the seqan_tcoffee program.

The golden test outputs are generated by the script generate_outputs.sh.

You have to give the root paths to the source and the binaries as arguments to
the program.  These are the paths to the directory that contains the 'projects'
directory.

Usage:  run_tests.py SOURCE_ROOT_PATH BINARY_ROOT_PATH
"""
import logging
import os.path
import sys
import platform

# Automagically add util/py_lib to PYTHONPATH environment variable.
path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..',
                                    '..', '..', 'util', 'py_lib'))
sys.path.insert(0, path)

import seqan.app_tests as app_tests

def main(source_base, binary_base):
    """Main entry point of the script."""

    if platform.machine().startswith('mips') or platform.machine().startswith('s390'):
        print('Skipping tests for seqan_tcoffee on mips* and s390*')
        print('===================================================')
        return 0

    print('Executing test for seqan_tcoffee')
    print('================================')
    print()

    ph = app_tests.TestPathHelper(
        source_base, binary_base,
        'apps/seqan_tcoffee/tests')  # tests dir

    # ============================================================
    # Auto-detect the binary path.
    # ============================================================

    path_to_program = app_tests.autolocateBinary(
      binary_base, 'apps/seqan_tcoffee', 'seqan_tcoffee')

    # ============================================================
    # Built TestConf list.
    # ============================================================

    # Build list with TestConf objects, analoguely to how the output
    # was generated in generate_outputs.sh.
    conf_list = []

    # ============================================================
    # Run on Proteins (Balibase).
    # ============================================================

    # Run with defaults for all non-mandatory options.
    for fname in ['1aab', '1ad2', '2trx']:
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.fasta' % fname)],
            to_diff=[(ph.inFile('%s.fasta' % fname),
                      ph.outFile('%s.fasta' % fname))])
        conf_list.append(conf)

    # Run with explicit alphabet.
    for fname in ['1aab', '1ad2', '2trx']:
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-a', 'protein',
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.protein.fasta' % fname)],
            to_diff=[(ph.inFile('%s.protein.fasta' % fname),
                      ph.outFile('%s.protein.fasta' % fname))])
        conf_list.append(conf)

        # Run with different segment match generation options.  We run
    # with with single values and combinations of neighbours
    for fname in ['1aab', '1ad2', '2trx']:

        for m in ['global', 'local', 'overlap', 'lcs']:
            conf = app_tests.TestConf(
                program=path_to_program,
                args=['-m', m,
                      '-s', ph.inFile('%s.fa' % fname),
                      '-o', ph.outFile('%s.m%s.fasta' % (fname, m))],
                to_diff=[(ph.inFile('%s.m%s.fasta' % (fname, m)),
                          ph.outFile('%s.m%s.fasta' % (fname, m)))])
            conf_list.append(conf)
        m1 = 'global'
        m2 = 'local'
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-m', m1,
                  '-m', m2,
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.m%s.m%s.fasta' % (fname, m1, m2))],
            to_diff=[(ph.inFile('%s.m%s.m%s.fasta' % (fname, m1, m2)),
                      ph.outFile('%s.m%s.m%s.fasta' % (fname, m1, m2)))])
        conf_list.append(conf)
        m1 = 'local'
        m2 = 'overlap'
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-m', m1,
                  '-m', m2,
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.m%s.m%s.fasta' % (fname, m1, m2))],
            to_diff=[(ph.inFile('%s.m%s.m%s.fasta' % (fname, m1, m2)),
                      ph.outFile('%s.m%s.m%s.fasta' % (fname, m1, m2)))])
        conf_list.append(conf)

        m1 = 'overlap'
        m2 = 'lcs'
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-m', m1,
                  '-m', m2,
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.m%s.m%s.fasta' % (fname, m1, m2))],
            to_diff=[(ph.inFile('%s.m%s.m%s.fasta' % (fname, m1, m2)),
                      ph.outFile('%s.m%s.m%s.fasta' % (fname, m1, m2)))])
        m1 = 'global'
        m2 = 'lcs'
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-m', m1,
                  '-m', m2,
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.m%s.m%s.fasta' % (fname, m1, m2))],
            to_diff=[(ph.inFile('%s.m%s.m%s.fasta' % (fname, m1, m2)),
                      ph.outFile('%s.m%s.m%s.fasta' % (fname, m1, m2)))])

    # Run with different match files variations.
    # TODO

    # Run with different scoring options.
    for fname in ['1aab', '1ad2', '2trx']:
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-g', '-20',
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.g-20.fasta' % fname)],
            to_diff=[(ph.inFile('%s.g-20.fasta' % fname),
                      ph.outFile('%s.g-20.fasta' % fname))])
        conf_list.append(conf)
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-e', '-5',
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.e-5.fasta' % fname)],
            to_diff=[(ph.inFile('%s.e-5.fasta' % fname),
                      ph.outFile('%s.e-5.fasta' % fname))])
        conf_list.append(conf)
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-ms', '10',
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.ms10.fasta' % fname)],
            to_diff=[(ph.inFile('%s.ms10.fasta' % fname),
                      ph.outFile('%s.ms10.fasta' % fname))])
        conf_list.append(conf)
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-mm', '-8',
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.mm-8.fasta' % fname)],
            to_diff=[(ph.inFile('%s.mm-8.fasta' % fname),
                      ph.outFile('%s.mm-8.fasta' % fname))])
        conf_list.append(conf)

    # Run with matrix file.
    for fname in ['1aab', '1ad2', '2trx']:
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-ma', ph.inFile('VTML200I'),
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.maVTML200.fasta' % fname)],
            to_diff=[(ph.inFile('%s.maVTML200.fasta' % fname),
                      ph.outFile('%s.maVTML200.fasta' % fname))])
        conf_list.append(conf)

    # Run with manual guide tree.
    for fname in ['1aab', '1ad2', '2trx']:
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-u', ph.inFile('%s.newick' % fname),
                  '-s', ph.inFile('%s.fa' % fname),
                  '-o', ph.outFile('%s.u.fasta' % fname)],
            to_diff=[(ph.inFile('%s.u.fasta' % fname),
                      ph.outFile('%s.u.fasta' % fname))])
        conf_list.append(conf)

    # Run with different guide tree building options.
    for fname in ['1aab', '1ad2', '2trx']:
        for b in ['nj', 'min', 'max', 'avg', 'wavg']:
            conf = app_tests.TestConf(
                program=path_to_program,
                args=['-b', b,
                      '-s', ph.inFile('%s.fa' % fname),
                      '-o', ph.outFile('%s.b%s.fasta' % (fname, b))],
                to_diff=[(ph.inFile('%s.b%s.fasta' % (fname, b)),
                          ph.outFile('%s.b%s.fasta' % (fname, b)))])
            conf_list.append(conf)

    # Run alignment evaluation.
    for fname in ['1aab', '1ad2', '2trx']:
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-i', ph.inFile('%s.fasta' % fname)],
            redir_stdout=ph.outFile('%s.i.fasta' % fname),
            to_diff=[(ph.inFile('%s.i.fasta' % fname),
                      ph.outFile('%s.i.fasta' % fname))])
        conf_list.append(conf)

    # ============================================================
    # Run on DNA (Adenoviruses).
    # ============================================================

    # Run with defaults for all non-mandatory options.
    for i in [2, 3, 4]:
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-a', 'dna',
                  '-s', ph.inFile('adeno%d.fa' % i),
                  '-o', ph.outFile('adeno%d.fasta' % i)],
            to_diff=[(ph.inFile('adeno%d.fasta' % i),
                      ph.outFile('adeno%d.fasta' % i))])
        conf_list.append(conf)

    # ============================================================
    # Run on RNA.
    # ============================================================

    # Run with defaults for all non-mandatory options.
    for i in [2, 3, 4]:
        conf = app_tests.TestConf(
            program=path_to_program,
            args=['-a', 'rna',
                  '-s', ph.inFile('adeno%d-rna.fa' % i),
                  '-o', ph.outFile('adeno%d-rna.fasta' % i)],
            to_diff=[(ph.inFile('adeno%d-rna.fasta' % i),
                      ph.outFile('adeno%d-rna.fasta' % i))])
        conf_list.append(conf)

    # Execute the tests.
    failures = 0
    for conf in conf_list:
        res = app_tests.runTest(conf)
        # Output to the user.
        print(' '.join(['seqan_tcoffee'] + conf.args), end=' ')
        if res:
             print('OK')
        else:
            failures += 1
            print('FAILED')

    # Cleanup.
    ph.deleteTempDir()

    print('==============================')
    print('     total tests: %d' % len(conf_list))
    print('    failed tests: %d' % failures)
    print('successful tests: %d' % (len(conf_list) - failures))
    print('==============================')
    # Compute and return return code.
    return failures != 0


if __name__ == '__main__':
    sys.exit(app_tests.main(main))
