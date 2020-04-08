#!/usr/bin/env python
# example.py

from PandoraBDT import *

###Event features are as follows:
#Event Showeryness
#Event Energy
#Event Volume
#Longitudinality
#Number of Hits
#Number of Clusters
#Number of Candidate Vertices

###Vertex features (x2 for best&chosen candidates) are as follows:
#Beam DeWeighting
#Enery Kick
#Local Asymmetry
#Global Asymmetry
#Shower Asymmetry
#(rPhi Score - used in vertex finding)

if __name__=="__main__":

    # Settings ------------------------------------------------------------------------------------
    
    #trainingFile      = '/Path/To/Training/Samples.txt'
    trainingFile      = '/icarus/app/users/rhowell/dev_build/v08_41_00_prof/srcs/icaruscode/icaruscode/LArMachineLearningData/VertexSelection_Vertex_combined.txt'
    #trainingMomentum  = 3
    regionOrVertex    = 'Vertex'
    testingFiles      = []
    #testingFiles      = [
    #                        {'File' : '/Path/To/Testing/Samples_1.txt', 'Momentum': 1 },
    #                        {'File' : '/Path/To/Testing/Samples_2.txt', 'Momentum': 2 }
    #                    ]
    bdtName           = 'BdtExample'
    treeDepth         = int(sys.argv[1]) 
    nTrees            = int(sys.argv[2]) 
    #trainTestSplit    = 0.5
    trainTestSplit    = 0.33

    plotFeatures      = False # Draws distributions of signal and background class features, then exits
    serializeToPkl    = True 
    serializeToXml    = True 
    loadFromPkl       = False 
    makeScorePlots    = True # Makes plots of BDT score for training and testing samples 
    xmlFileName       = bdtName + '_' + str(regionOrVertex) + '_NTrees_' + str(nTrees) + '_TreeDepth_' + str(treeDepth) + '.xml'
    pklFileName       = bdtName + '_' + str(regionOrVertex) + '_NTrees_' + str(nTrees) + '_TreeDepth_' + str(treeDepth) + '.pkl'
    
    #----------------------------------------------------------------------------------------------

    if plotFeatures:
        # Load the training data
        OverwriteStdout('Loading training set data for plotting from file ' + trainingFile + '\n')
        trainSet, nFeatures, nExamples = LoadData(trainingFile, ',')
        X_org, Y_org = SplitTrainingSet(trainSet, nFeatures)

        # Plot Variables then Exit
        DrawVariables(X_org, Y_org)
        Correlation(X_org, Y_org)
        sys.exit()

    if loadFromPkl:
        OverwriteStdout('Loading model from file ' + pklFileName + '\n')
        bdtModel = LoadFromPkl(pklFileName)
    
    else:
        # Load the training data
        OverwriteStdout('Loading training set data from file ' + trainingFile + '\n')
        trainSet, nFeatures, nExamples = LoadData(trainingFile, ',')
        X_org, Y_org = SplitTrainingSet(trainSet, nFeatures)
        
        # Train the BDT
        X, Y = Randomize(X_org, Y_org)
        X_train, Y_train, X_test, Y_test = Sample(X, Y, trainTestSplit)

        OverwriteStdout('Training AdaBoostClassifer...')
        bdtModel, trainingTime = TrainAdaBoostClassifer(X_train, Y_train, n_estimatorsValue=nTrees, max_depthValue=treeDepth)

        OverwriteStdout(('Trained AdaBoostClassifer with ' + str(nFeatures) + ' features and ' + 
                         str(nExamples) + ' examples (%d seconds, %i TreeDepth, %i nTrees)\n' % (trainingTime, treeDepth, nTrees)))
                  
        # Validate the model 
        modelScore = ValidateModel(bdtModel, X_test, Y_test)
        OverwriteStdout('Model score: %.2f%%\n' % (modelScore * 100))
        
        if serializeToXml:
            OverwriteStdout('Writing model to xml file ' + xmlFileName + '\n')
            datetimeString = datetime.now().strftime('%Y-%m-%d_%H:%M:%S')
            WriteXmlFile(xmlFileName, bdtModel)

        if serializeToPkl:
            OverwriteStdout('Writing model to pkl file ' + pklFileName + '\n')
            SerializeToPkl(pklFileName, bdtModel)
            
    # Do other stuff with your trained/loaded model
    # ...

    if makeScorePlots:
        parameters = {
                         #'ClassNames':['Beam Particle','Cosmic Rays'],
                         'ClassNames':['Right Vtx','Wrong Vtx'],
                         'SignalDefinition': [1, 0],
                         'PlotColors': ['b', 'r'], 
                         'nBins': 100,
                         'PlotStep': 1.0,
                         'OptimalBinCut': 0,
                         'OptimalScoreCut': 0.0,
                         'nTrees': nTrees,
                         'TreeDepth': treeDepth,
                         'EdgeColors': ['seagreen','mediumorchid']
                     }

        FindOptimalSignificanceCut(bdtModel, X_train, Y_train, parameters) 
        PlotBdtScores(bdtModel, X_test, Y_test, regionOrVertex, parameters)
        PlotBdtScoresTrainTest(bdtModel, X_train, Y_train, X_test, Y_test, regionOrVertex, parameters)

        #for idx, testFile in enumerate(testingFiles):
        #    testSet, nFeaturesTest, nExamplesTest = LoadData(testFile['File'], ',')
        #    X_test_data, Y_test_data = SplitTrainingSet(testSet, nFeaturesTest)
        #    PlotBdtScores(bdtModel, X_test_data, Y_test_data, testFile['Momentum'], parameters)

