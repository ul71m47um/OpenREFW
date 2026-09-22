STDLIB=/usr/share/libc++/v1/std.cppm
STDPCM=$(addprefix $(OBJS)/, $(notdir $(STDLIB:.cppm=.pcm)))
STDOBJ=$(STDLIB:.pcm=.o)

CXX=clang++
CXXFLAGS=-std=c++26 -O3 -c $(OTHER_CPLUSPLUSFLAGS) -fprebuilt-module-path=$(OBJS) -fcxx-modules

AR=ar
ARFLAGS=rcs

LIB=$(OBJS)/libcore.a

MODULES=\
	./modules/disxx-utility/private/error/disxx.utility.error.ParserError.cppm \
	./modules/disxx-utility/private/error/disxx.utility.error.NullPointerError.cppm \
	./modules/disxx-utility/private/error/disxx.utility.error.DisassemblyError.cppm \
	./modules/disxx-utility/private/ini/disxx.utility.ini.BaseParser.cppm \
	./modules/disxx-utility/private/ini/disxx.utility.ini.Reader.cppm \
	./modules/disxx-utility/private/ini/disxx.utility.ini.Writer.cppm \
	./modules/disxx-utility/private/pointer/disxx.utility.pointer.DefaultDeleter.cppm \
	./modules/disxx-utility/public/file/disxx.utility.file.MappedFile.cppm \
	./modules/disxx-utility/public/disxx.utility.Overload.cppm \
	./modules/disxx-utility/public/ini/disxx.utility.ini.Parser.cppm \
	./modules/disxx-utility/public/trait/disxx.utility.trait.IClone.cppm \
	./modules/disxx-utility/public/pointer/disxx.utility.pointer.Pointer.cppm \
	./modules/disxx-utility/public/pointer/disxx.utility.pointer.Weak.cppm \
	./modules/disxx-utility/public/pointer/disxx.utility.pointer.NonNull.cppm \
	./modules/disxx-loader/private/executable/disxx.loader.executable.Label.cppm \
	./modules/disxx-loader/private/executable/disxx.loader.executable.Section.cppm \
	./modules/disxx-loader/public/executable/disxx.loader.executable.ExecutableFile.cppm \
	./modules/disxx-loader/private/utility/disxx.loader.utility.BinaryInfo.cppm \
	./modules/disxx-loader/public/disxx.loader.macho.Loader.cppm \
	./modules/disxx-disasm/private/disxx.disasm.InstructionIdentifier.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.IOperand.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.SystemOperand.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.Register-VectorArrangementSpecifier.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.PState.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.PrefetchOperand.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.Extension.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.Immediate.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.MemoryBarrier.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.Shift.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.Register.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.LoadsAndStoresAddress.cppm \
	./modules/disxx-disasm/private/operand/disxx.disasm.operand.Condition.cppm \
	./src/disxx-disasm/utility/bits/disxx.disasm.utility.bits.cppm \
	./src/disxx-disasm/decoder/abstract/disxx.disasm.decoder.abstract.SubDecoder.cppm \
	./src/disxx-disasm/decoder/abstract/disxx.disasm.decoder.abstract.Decoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.EvaluateIntoFlags.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.ConditionalCompareImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.AddSubstractCheckedPointer.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.ConditionalSelect.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.DataProcessing1Source.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.DataProcessing3Source.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.ConditionalCompareRegister.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.AddSubstractWithCarry.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.RotateRightIntoFlags.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.LogicalShiftedRegister.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.AddSubstractShiftedRegister.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.DataProcessing2Source.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/SubDecoder/disxx.disasm.decoder.DataProcessingRegister.AddSubstractExtendedRegister.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingRegister/disxx.disasm.decoder.DataProcessingRegister.Decoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/SubDecoder/disxx.disasm.decoder.DataProcessingImmediate.MinMaxImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/SubDecoder/disxx.disasm.decoder.DataProcessingImmediate.Bitfield.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/SubDecoder/disxx.disasm.decoder.DataProcessingImmediate.LogicalImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/SubDecoder/disxx.disasm.decoder.DataProcessingImmediate.AddSubstractImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/SubDecoder/disxx.disasm.decoder.DataProcessingImmediate.DataProcessing1SourceImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/SubDecoder/disxx.disasm.decoder.DataProcessingImmediate.PCRelAddressing.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/SubDecoder/disxx.disasm.decoder.DataProcessingImmediate.AddSubstractImmediateWithTags.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/SubDecoder/disxx.disasm.decoder.DataProcessingImmediate.MoveWideImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/SubDecoder/disxx.disasm.decoder.DataProcessingImmediate.Extract.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingImmediate/disxx.disasm.decoder.DataProcessingImmediate.Decoder.cppm \
	./src/disxx-disasm/decoder/Reserved/SubDecoder/disxx.disasm.decoder.Reserved.UDF.SubDecoder.cppm \
	./src/disxx-disasm/decoder/Reserved/disxx.disasm.decoder.Reserved.Decoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDModifiedImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarThreeSameExtra.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDVectorXIndexedElement.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.CryptographicThreeRegisterSHA512.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDThreeSame.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.ConversionBetweenFloatingPointAndFixedPoint.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarThreeDifferent.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDCopy.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.FloatingPointConditionalCompare.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarShiftByImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.FloatingPointDataProcessing2Source.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDThreeRegisterExtension.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.FloatingPointImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.XAR.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarThreeSameFP16.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.CryptographicTwoRegisterSHA512.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.CryptographicThreeRegisterSHA.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDThreeSameFP16.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDThreeDifferent.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDAcrossLanes.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarCopy.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDShiftByImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarPairwise.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarXIndexedElement.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarTwoRegisterMiscellaneousFP16.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.CryptographicTwoRegisterSHA.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.FloatingPointConditionalSelect.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDPermute.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarThreeSame.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDScalarTwoRegisterMiscellaneous.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.ConversionBetweenFloatingPointAndInteger.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDTwoRegisterMiscellaneous.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDExtract.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.CryptographicAES.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDTableLookup.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.FloatingPointDataProcessing3Source.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.AdvancedSIMDTwoRegisterMiscellaneousFP16.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.CryptographicThreeRegisterImm2.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.CryptographicFourRegister.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.FloatingPointCompare.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/SubDecoder/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.FloatingPointDataProcessing1Source.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DataProcessingScalarFPAndAdvancedSIMD/disxx.disasm.decoder.DataProcessingScalarFPAndAdvancedSIMD.Decoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/disxx.disasm.decoder.LoadsAndStores.Decoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.ExclusiveRegister.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.NoAllocatePairOffset.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.GCS.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.AtomicMemoryOperations.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.SIMDSingleStructurePostIndexed.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterPairPostIndexed.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterPAC.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.CompareAndSwapPairUnprivileged.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.LDAPRSTLRSIMDAndFP.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.CompareAndSwapUnprivileged.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterUnscaledImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterPairPreIndexed.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.SIMDMultipleStructures.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.LDAPRSTLRWriteback.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterUnprivileged.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.CompareAndSwapPair.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.AtomicMemoryOperationsUnprivileged.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.ExclusiveRegisterUnprivileged.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.MemoryCopyAndMemorySet.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.Ordered.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterImmediatePreIndexed.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterPairOffset.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterLiteral.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.LDAPRSTLRUnscaledImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.OneHundredAndTwentyEightBitAtomicMemoryOperations.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.LDIAPPSTILP.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.SIMDSingleStructure.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterUnsignedImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.SIMDMultipleStructuresPostIndexed.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RCWCompareAndSwapPair.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.CompareAndSwap.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RCWCompareAndSwap.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.MemoryTags.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterImmediatePostIndexed.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.ExclusivePair.SubDecoder.cppm \
	./src/disxx-disasm/decoder/LoadsAndStores/SubDecoder/disxx.disasm.decoder.LoadsAndStores.RegisterOffset.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.Decoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.SystemInstructionIDsWithRegisterArgument.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.SystemRegisterPairMove.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.ExceptionGeneration.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.UnconditionalBranchRegister.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.CompareAndBranchImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.CompareBytesHalfwordsInRegisterAndBranch.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.CompareRegisterWithImmediateAndBranch.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.Hints.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.TestAndBranchImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.CompareRegistersAndBranch.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.SystemPairInstructionIDs.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.MiscellaneousBranchImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.Barriers.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.SystemInstructionIDs.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.UnconditionalBranchImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.PSTATE.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.SystemWithResult.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.ConditionalBranchImmediate.SubDecoder.cppm \
	./src/disxx-disasm/decoder/BranchesExceptionsAndSystemInstructions/SubDecoder/disxx.disasm.decoder.BranchesExceptionsAndSystemInstructions.SystemRegisterMove.SubDecoder.cppm \
	./src/disxx-disasm/decoder/DecoderFactory/disxx.disasm.decoder.DecoderFactory.cppm \
	./src/disxx-disasm/decoder/DecoderFactory/disxx.disasm.decoder.DecoderFactory-PseudoDecoder.cppm \
	./modules/disxx-disasm/private/disxx.disasm.Bytes.cppm \
	./modules/disxx-disasm/private/disxx.disasm.Address.cppm \
	./modules/disxx-disasm/private/disxx.disasm.Instruction.cppm \
	./modules/disxx-disasm/public/disxx.disasm.Disassembler.cppm \
	./modules/disxx-disasm/public/disxx.disasm.Printer.cppm

PRECOMPILED=$(addprefix $(OBJS)/, $(notdir $(MODULES:.cppm=.pcm)))
OBJECTS=$(PRECOMPILED:.pcm=.o)

vpath %.cppm $(sort $(dir $(MODULES)))
vpath %.pcm $(sort $(dir $(OBJECTS)))

.PHONY: all
all: $(LIB)

$(STDPCM): $(STDLIB)
	mkdir -p $(OBJS)
	clang++ $(CXXFLAGS:-c=--precompile) $< -o $@
$(STDOBJ): $(STDPCM)
	mkdir -p $(OBJS)
	clang++ $(CXXFLAGS) $< -o $@

$(OBJS)/%.pcm: %.cppm | $(STDPCM)
	mkdir -p $(OBJS)
	clang++ $(CXXFLAGS:-c=--precompile) $< -o $@
$(OBJECTS): $(OBJS)/%.o: $(OBJS)/%.pcm | $(STDOBJ)
	mkdir -p $(OBJS)
	clang++ $(CXXFLAGS) -c $< -o $@

$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^ 
