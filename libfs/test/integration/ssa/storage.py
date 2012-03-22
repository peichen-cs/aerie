#
# STORAGE/CONTAINER TESTS
#

import testfw
import os 


def addIntegrationTests(env, parent_dir, testProgram, serverProgram):
    env.addIntegrationTest(testfw.integration_test.IntegrationTest(
        name = 'SSA_StorageSystem:Test',
        init_script = os.path.join(parent_dir, 'test/integration/init.sh'),
        testfw = testProgram, server = serverProgram,
        clients = { 
            'C1': ( testProgram, [('T1', 'SSA_StorageSystem:Test')]),
        },
        rendezvous = []
    ))
