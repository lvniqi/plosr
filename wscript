## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    module = bld.create_ns3_module('polsr', ['internet'])
    module.includes = '.'
    module.source = [
        'model/cached-queue.cc',
        'model/polsr-header.cc',
        'model/polsr-state.cc',
        'model/polsr-routing-protocol.cc',
        'helper/polsr-helper.cc',
        ]
    headers = bld(features='ns3header')
    headers.module = 'polsr'
    headers.source = [
        'model/cached-queue.h',
        'model/polsr-routing-protocol.h',
        'model/polsr-header.h',
        'model/polsr-state.h',
        'model/polsr-repositories.h',
        'helper/polsr-helper.h',
        ]


    #if bld.env['ENABLE_EXAMPLES']:
    #    bld.recurse('examples')

    #bld.ns3_python_bindings()
