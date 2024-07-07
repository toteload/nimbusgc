import subprocess
import os
import ninja_syntax as n

join = os.path.join

def list_files_in_dir(d):
    for f in os.listdir(d):
        p = os.path.join(d, f)
        if os.path.isfile(p):
            yield f

def create_build_ninja():
    fout = open("build.ninja", "w")
    out = n.Writer(fout)

    out._line('builddir = .ninja')

    out.variable(
        key   = 'msvc_deps_prefix',
        value = 'Note: including file:',
        )

    out.variable(
        key   = 'cflags',
        value = '-nologo -showIncludes -W4 -Oi',
        )

    out.variable(
        key   = 'debugcflags',
        value = '-Zi -MTd'
        )

    out.variable(
        key   = 'outdir',
        value = 'out',
        )

    out.rule(
        name    = 'compile_debug',
        deps    = 'msvc',
        command = 'clang-cl $cflags $debugcflags $extraflags -Fo$out -c $in',
        )

    out.rule(
        name    = 'build_exe',
        command = 'clang-cl -Zi $in -Fe:$out',
        )

    out.build(
        outputs = join('$outdir', 'catch_amalgamated.obj'),
        rule    = 'compile_debug',
        inputs  = 'test/catch_amalgamated.cpp',
        )

    out.build(
        outputs = join('$outdir', 'allocator.obj'),
        rule    = 'compile_debug',
        inputs  = 'src/allocator.cpp',
        )

    for f in list_files_in_dir('test'):
        base, _ = os.path.splitext(f);
        if not base.endswith('.test'):
            continue
        out.build(
            outputs = join('$outdir', base) + '.obj',
            rule    = 'compile_debug',
            inputs  = join('test', f),
            variables = {
                'extraflags': '-I src',
            },
            )

    out.build(
        outputs = join('$outdir', 'nimbus.test.exe'),
        rule    = 'build_exe',
        inputs  = [join('$outdir', p) for p in [ 
            'allocator.test.obj', 
            'bitmap.test.obj', 
            'virtualmemoryregion.test.obj', 
            'allocator.obj',
            'catch_amalgamated.obj' 
        ]],
        implicit_outputs = [join('$outdir', p) for p in [ 
            'nimbus.test.ilk',
            'nimbus.test.pdb',
        ]],
        )

    out.build(
        outputs = 'test',
        rule    = 'phony',
        implicit = join('$outdir', 'nimbus.test.exe'),
        )

    out.default('test')

if __name__ == "__main__":
    create_build_ninja()    
